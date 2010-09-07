// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#define RawConnTrace VLOG(2) << Name() << " : " << __func__ << " status: " << status->Status() << " "

#include <boost/thread.hpp>
#include <protobuf/service.h>

#include "Common.hpp"
#include "network/Connection.hpp"
#include "network/RawConnection.hpp"
#include "thread/Executor.hpp"
#include "Logger.hpp"
#include "proto/meta.pb.h"
#include "SharedConstBuffers.hpp"

class ExecuteHandler {
public:

    ExecuteHandler( const RawConnection::StatusPtr &status,
            RawConnection *connection,
            void (RawConnection::*member )( RawConnection::StatusPtr ) ) :
    status_( status ), connection_( connection ), member_( member ) { }

    void operator( )( ) {
        status_->Mutex( ).lock_shared( );

        if ( status_->IsClosing( ) ) {
            status_->Mutex( ).unlock_shared( );
            return;
        }
        
        ( connection_->*member_ )( status_ );
    }
private:
    RawConnection::StatusPtr status_;
    RawConnection *connection_;
    void (RawConnection::*member_ )( RawConnection::StatusPtr );
};

class ReadHandler {
public:

    ReadHandler( const RawConnection::StatusPtr &status,
            RawConnection *connection,
            void (RawConnection::*member )(
            RawConnection::StatusPtr status,
            const boost::system::error_code &, size_t ) ) :
    status_( status ), connection_( connection ), member_( member ) { }

    void operator( )( const boost::system::error_code &e, size_t n ) {

        status_->Mutex( ).lock_shared( );

        if ( status_->IsClosing( ) ) {
            status_->Mutex( ).unlock_shared( );
            return;
        }
        
        ( connection_->*member_ )( status_, e, n );
    }
private:
    RawConnection::StatusPtr status_;
    RawConnection *connection_;
    void (RawConnection::*member_ )(
            RawConnection::StatusPtr status, const boost::system::error_code&, size_t );
};

typedef ReadHandler WriteHandler;

RawConnection::RawConnection( const string &name, ConnectionPtr connection ) :
    name_( name ),
    incoming_index_( 0 ),
    connection_( connection ) { }

void
RawConnection::InitSocket( StatusPtr status, TcpSocket *socket ) {
    RawConnectionStatus::Locker locker( status->Mutex( ) );
    assert( !status->IsClosing( ) );
    RawConnTrace;
    mSocket.reset( socket );
    boost::asio::socket_base::keep_alive    keep_alive( true );
    mSocket->set_option( keep_alive );
    boost::asio::socket_base::linger        linger( false, 0 );
    mSocket->set_option( linger );
    // Put the socket into non-blocking mode.
    TcpSocket::non_blocking_io              non_blocking_io( true );
    mSocket->io_control( non_blocking_io );
    send_package_ = 0;
    recv_package_ = 0;
    StartOOBRecv( status );
    status->SetReading( );
    ReadHandler h( status, this, &RawConnection::HandleRead );
    mSocket->async_read_some( boost::asio::buffer( mBuffer ), h );
}

void
RawConnection::Disconnect( StatusPtr status, bool async ) {
    boost::shared_mutex *mut = &status->Mutex( );

    if ( status->IsClosing() ) {
        VLOG( 2 ) << "Already closing";
        mut->unlock_shared( );
        return;
    }
    
    mut->unlock_shared( );
    mut->lock_upgrade( );
    mut->unlock_upgrade_and_lock( );

    if ( status->IsClosing() ) {
        VLOG( 2 ) << "Already closing";
        mut->unlock( );
        return;
    }
    
    status->SetClosing();
    mut->unlock( );
    
    if ( mSocket.get( ) ) {
        mSocket->close( );
        mSocket.reset( );
    }
    
    ConnectionPtr conn = connection_;
    connection_.reset( );

    if ( async ) {
        conn->ImplClosed( );
    }
}

RawConnection::~RawConnection( ) {
    VLOG( 2 ) << Name( ) << "~RawConnection";
}

void
RawConnection::Heartbeat( StatusPtr status ) {
    RawConnTrace << "Heartbeat send_package: " << send_package_ << " recv_package: " << recv_package_;
    char heartbeat = kHeartBeat;
    boost::system::error_code ec;
    int n = mSocket->send( boost::asio::buffer( &heartbeat, sizeof (heartbeat ) ),
            boost::asio::socket_base::message_out_of_band, ec );
    
    if ( ec || ( n != sizeof (heartbeat ) ) ) {
        RawConnTrace << "OOBSend error, n:" << n << " ec: " << ec.message( );
        Disconnect( status, true );
        return;
    }

    ++send_package_;
    
    if ( abs( send_package_ - recv_package_ ) > kHeartbeatUnsyncWindow ) {
        RawConnTrace << "send_package_: " << send_package_ << " recv_package_: " << recv_package_ << " not synced, disconnect";
        Disconnect( status, true );
        return;
    }
    
    status->Mutex( ).unlock_shared( );
}

void
RawConnection::StartOOBRecv( StatusPtr status ) {
    ReadHandler h( status, this, &RawConnection::OOBRecv );
    mSocket->async_receive( boost::asio::buffer( &heartbeat_, sizeof (heartbeat_ ) ),
            boost::asio::socket_base::message_out_of_band, h );
}

void
RawConnection::OOBRecv(
        StatusPtr status,
        const boost::system::error_code &e, size_t n ) {
    RawConnTrace << "OOBRecv send_package: " << send_package_ << " recv_package: " << recv_package_ << " e: " << e.message( );

    if ( e ) {
        Disconnect( status, true );
        return;
    }

    ++recv_package_;
    
    StartOOBRecv( status );
    status->Mutex( ).unlock_shared( );
}

void
RawConnection::HandleRead( StatusPtr status,
        const boost::system::error_code& e,
        size_t bytes_transferred ) {

    assert( status->IsReading() );
    RawConnTrace << " e:" << e.message( ) << " bytes: " << bytes_transferred;

    if ( e ) {
        status->ClearReading();
        Disconnect( status, true );
        return;
    }
    
    if ( !Decode( bytes_transferred ) ) {
        RawConnTrace << "Decoder error";
        status->ClearReading();
        Disconnect( status, true );
        return;
    }
    
    ReadHandler h( status, this, &RawConnection::HandleRead );
    mSocket->async_read_some( boost::asio::buffer( mBuffer ), h );
    status->Mutex( ).unlock_shared( );
}

bool
RawConnection::ScheduleWrite( StatusPtr status ) {
    RawConnTrace;
    boost::mutex::scoped_lock incoming_locker( incoming_mutex_ );

    if ( status->IsWriting() ) {
        RawConnTrace << " : " << "ScheduleWrite but already writting";
        return true;
    }

    status->SetWriting();
    RawConnTrace << "duplex_[0] : " << duplex_[0].empty( );
    RawConnTrace << "duplex_[1] : " << duplex_[1].empty( );
    RawConnTrace << "incoming: " << incoming_index_;
    SwitchIO( );

    if ( outcoming( )->empty( ) ) {
        status->ClearWriting();
        RawConnTrace << "No outcoming";
        return true;
    }

    WriteHandler h( status, this, &RawConnection::HandleWrite );
    mSocket->async_write_some( *outcoming( ), h );
    return true;
}

void
RawConnection::HandleWrite( StatusPtr status,
        const boost::system::error_code& e, size_t byte_transferred ) {
    assert( status->IsWriting() );
    RawConnTrace << "e:" << e.message( ) << " byte_transferred: " << byte_transferred;

    if ( e ) {
        status->ClearWriting();
        Disconnect( status, true );
        return;
    }
    
    boost::mutex::scoped_lock locker( incoming_mutex_ );
    outcoming( )->consume( byte_transferred );

    if ( !outcoming( )->empty( ) ) {
        WriteHandler h( status, this, &RawConnection::HandleWrite );
        mSocket->async_write_some( *outcoming( ), h );
    } else {
        RawConnTrace << "outcoming is empty";
        SwitchIO( );

        if ( !outcoming( )->empty( ) ) {
            RawConnTrace << " : " << "outcoming is not empty after SwitchIO, size :" << outcoming( )->size( );
            WriteHandler h( status, this, &RawConnection::HandleWrite );
            mSocket->async_write_some( *outcoming( ), h );
            
        } else {
            status->ClearWriting();
        }
    }

    status->Mutex( ).unlock_shared( );
}
#undef RawConnTrace
