// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)
#include <boost/bind.hpp>
#include "Logger.hpp"
#include "Network.hpp"
#include "Server.hpp"
//common
#include "network/ProtobufConnection.hpp"
#include "network/IOServicePool.hpp"

class AcceptorHandler {
public:

    AcceptorHandler( TcpAcceptor *acceptor,
            TcpSocket **socket_pptr,
            const string host,
            Server *server,
            Connection *connection_template ) :

                    acceptor_( acceptor ),
                    socket_pptr_( socket_pptr ),
                    host_( host ),
                    server_( server ),
                    connection_template_( connection_template )
    { }

    void operator()( const boost::system::error_code& e ) {

        if ( !e ) {
            VLOG( 2 ) << "HandleAccept " << host_;
            TcpSocket *socket = *socket_pptr_;

            if ( socket && socket->is_open() ) {
                VLOG( 2 ) << "Socket is connected";
                IOService &io_service = socket->get_io_service();
                *socket_pptr_ = new TcpSocket( io_service );
                acceptor_->async_accept( **socket_pptr_, *this );
                server_->HandleAccept( e, socket, connection_template_ );

            } else {
                acceptor_->async_accept( **socket_pptr_, *this );
            }

        } else {
            VLOG( 1 ) << "HandleAccept error: " << e.message();
            server_->ReleaseAcceptor( host_ );
        }
    }

private:
    string host_;
    // Have the ownership.
    TcpAcceptor *acceptor_;
    TcpSocket **socket_pptr_;
    // Haven't the ownership.
    Server *server_;
    Connection *connection_template_;
};

Server::~Server() {
    if ( io_service_pool_.IsRunning() ) {
        Stop();
    }
}

Server::Server(
        int io_service_number,
        int worker_threads,
        long drain_timeout ) :
io_service_pool_( "ServerIOService", io_service_number, worker_threads ),
notifier_( new Notifier( "ServerNotifier", 1 ) ),
drain_timeout_( drain_timeout ) {}

void
Server::ReleaseAcceptor( const string &host ) {
    boost::mutex::scoped_lock locker( acceptor_table_mutex_ );
    AcceptorTable::iterator it = acceptor_table_.find( host );

    if ( it == acceptor_table_.end() ) {
        VLOG( 2 ) << "Can't find " << host;
        return;
    }

    it->second.Release();
}

void
Server::Listen( const string &address, const string &port,
        Connection* connection_template ) {
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    VLOG( 2 ) << "Server running";
    io_service_pool_.Start();
    timer_master_.Start();
    const string host( address + "::" + port );
    
    TcpAcceptor *acceptor = new TcpAcceptor( io_service_pool_.GetIOService() );
    TcpResolver     resolver( acceptor->io_service() );
    TcpResolver::query query( address, port );
    TcpEndpoint     endpoint = *resolver.resolve( query );

    acceptor->open( endpoint.protocol() );
    acceptor->set_option( TcpAcceptor::reuse_address( true ) );
    acceptor->bind( endpoint );
    acceptor->listen();

    TcpSocket **socket_pptr = new (TcpSocket* );
    *socket_pptr = new TcpSocket( io_service_pool_.GetIOService() );
    {
        boost::mutex::scoped_lock locker( acceptor_table_mutex_ );
        acceptor_table_.insert( make_pair( host, AcceptorResource( acceptor, socket_pptr ) ) );
    }
    acceptor->async_accept( **socket_pptr, AcceptorHandler( acceptor, socket_pptr, host, this, connection_template ) );
}

void
Server::Stop() {
    VLOG( 2 ) << "Server stop";
    stop_mutex_.lock();

    if ( !io_service_pool_.IsRunning() ) {
        VLOG( 2 ) << "Server already stopped";
        stop_mutex_.unlock();
        return;
    }
    
    {
        boost::mutex::scoped_lock locker( channel_table_mutex_ );

        for ( ChannelTable::iterator it = channel_table_.begin(); it != channel_table_.end(); ++it ) {
            VLOG( 2 ) << "Close: " << ( *it )->Name();
            ( *it )->Disconnect();
            notifier_->Dec( 1 );
        }

        notifier_->Dec( 1 );
        channel_table_.clear();
    }
    // All channel had been flushed.
    notifier_->Wait();
    {
        boost::mutex::scoped_lock locker( acceptor_table_mutex_ );

        for ( AcceptorTable::iterator it = acceptor_table_.begin(); it != acceptor_table_.end(); ++it ) {
            VLOG( 2 ) << "Delete acceptor on " << it->first;
            it->second.Release();
        }

        acceptor_table_.clear();
    }
    
    LOG( WARNING ) << "Stop io service pool";
    io_service_pool_.Stop();
    stop_mutex_.unlock();
    timer_master_.Stop();
    LOG( WARNING ) << "Server stopped";
    CHECK( channel_table_.empty() );
}

void
Server::ConnectionClosed( Connection *connection ) {
    boost::mutex::scoped_lock locker( channel_table_mutex_ );
    int size1 = channel_table_.size();
    ConnectionPtr conn = connection->shared_from_this();
    VLOG( 2 ) << "ConnectionClosed: " << conn.get();

    if ( channel_table_.find( conn ) != channel_table_.end() ) {
        channel_table_.erase( conn );
        notifier_->Dec( 1 );
        VLOG( 1 ) << "Remove connection:" << connection->Name();
    } else {
        VLOG( 1 ) << "Had removed connection:" << connection->Name();
    }
}

void
Server::HandleAccept( const boost::system::error_code& e, TcpSocket *socket,
        Connection *connection_template ) {
    VLOG( 2 ) << "HandleAccept";
    stop_mutex_.lock_shared();

    if ( !io_service_pool_.IsRunning() ) {
        delete socket;
        stop_mutex_.unlock_shared();
        VLOG( 2 ) << "HandleAccept but already stopped.";
        return;
    }
    
    // The socket ownership transfer to Connection.
    ConnectionPtr connection = connection_template->Span( socket );

    if ( connection.get() == NULL ) {
        LOG( WARNING ) << "Span a NULL connection!";
        return;
    }
    
    {
        boost::mutex::scoped_lock locker( channel_table_mutex_ );
        boost::shared_ptr< Server > s = this->shared_from_this();

        if ( !connection->RegisterAsyncCloseListener( s ) ) {
            LOG( WARNING ) << "RegisterAsyncCloseListener failed";
            connection->Disconnect();
            return;
        }
        
        channel_table_.insert( connection );
        timer_master_.Register( connection );
        VLOG( 2 ) << "Insert: " << connection.get();
        notifier_->Inc( 1 );
    }
    VLOG( 2 ) << "Insert " << connection->Name();
    stop_mutex_.unlock_shared();
}
