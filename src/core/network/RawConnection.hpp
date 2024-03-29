/* 
 * File:   RawConnection.hpp
 * Author: vaulter
 *
 * Created on 6 РЎРµРЅС‚СЏР±СЂСЊ 2010 Рі., 3:09
 */

#ifndef RAWCONNECTION_HPP
#define RAWCONNECTION_HPP

// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

//#include "base/base.hpp"
#include "Atomic.hpp"
#include "Network.hpp"
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr.hpp>

using boost::scoped_ptr;

class RawConnectionStatus {
public:
    typedef boost::shared_lock< boost::shared_mutex > Locker;

    RawConnectionStatus() : status_( 0 ), mIntrusiveCount( 0 ) { }

    ~RawConnectionStatus() {
        CHECK_EQ( mIntrusiveCount, 0 );
    }

    bool
    IsReading() const {
        return status_ & READING;
    }

    bool
    IsWriting() const {
        return status_ & WRITTING;
    }

    void
    SetReading() {
        atomic_or( &status_, READING );
    }

    void
    SetWriting() {
        atomic_or( &status_, WRITTING );
    }

    void
    ClearReading() {
        atomic_and( &status_, ~READING );
    }

    void
    ClearWriting() {
        atomic_and( &status_, ~WRITTING );
    }

    void
    SetClosing() {
        atomic_or( &status_, CLOSING );
    }

    bool
    IsClosing() const {
        return status_ & CLOSING;
    }

    bool
    IsIdle() const {
        return status_ == IDLE;
    }

    int
    Status() const {
        return status_;
    }

    boost::shared_mutex &
    Mutex() {
        return mutex_;
    }
private:

    enum InternalRawConnectionStatus {
        IDLE = 0x0,
        READING = 0x01,
        WRITTING = 0x01 << 1,
        CLOSING = 0x01 << 2,
    };
    volatile int status_;
    volatile int mIntrusiveCount;
    template <class T> friend void intrusive_ptr_add_ref( T *t );
    template <class T> friend void intrusive_ptr_release( T *t );
    boost::shared_mutex mutex_;
};

class Connection;

class RawConnection : public boost::noncopyable {
private:
    typedef RawConnectionStatus::Locker Locker;
public:
    typedef boost::intrusive_ptr< RawConnectionStatus > StatusPtr;
    
    RawConnection( const string &name, boost::shared_ptr< Connection > connection );

    void Disconnect( StatusPtr status, bool async );

    bool ScheduleWrite( StatusPtr status );
    // The push will take the ownership of the data

    template <typename T>
    inline bool
    PushData( const T &data ) {
        boost::mutex::scoped_lock locker_incoming( incoming_mutex_ );
        InternalPushData( data );
        return true;
    }
    void InitSocket( StatusPtr status, TcpSocket *socket );

    const string
    Name() const {
        return name_;
    }
    virtual ~RawConnection();
protected:
    static const char kHeartBeat = 0xb;
    static const int kDefaultTimeoutMs = 30000;
    static const int kRecvDelayFactor = 2;
    template < class T > void InternalPushData( const T &data );

    SharedConstBuffers *
    incoming() {
        return &duplex_[ incoming_index_ ];
    }

    SharedConstBuffers *
    outcoming() {
        return &duplex_[1 - incoming_index_];
    }

    void
    SwitchIO() {
        //    CHECK(outcoming()->empty());
        // Switch the working vector.
        incoming_index_ = 1 - incoming_index_;
    }
    
    inline void OOBRecv( StatusPtr status, const boost::system::error_code &e, size_t n );
    inline void OOBSend( StatusPtr status, const boost::system::error_code &e );
    inline void HandleRead( StatusPtr status, const boost::system::error_code& e, size_t bytes_transferred );
    inline void HandleWrite( StatusPtr status, const boost::system::error_code& e, size_t byte_transferred );
    virtual bool Decode( size_t byte_transferred ) = 0;
    void StartOOBRecv( StatusPtr status );
    void Heartbeat( StatusPtr status );
    scoped_ptr< TcpSocket >     mSocket;
    string name_;

    char heartbeat_;

    static const int kBufferSize = 8192;
    static const int kHeartbeatUnsyncWindow = 2;
    typedef boost::array< char, kBufferSize > BufferType;
    BufferType                      mBuffer;

    int                             incoming_index_;
    SharedConstBuffers              duplex_[ 2 ];
    boost::mutex                    incoming_mutex_;
    boost::shared_ptr< Connection > connection_;

    int send_package_;
    int recv_package_;
    friend class Connection;
};
// Represents a protocol implementation.

template < typename Decoder >
class RawConnectionImpl : public RawConnection {
public:

    RawConnectionImpl( const string &name, boost::shared_ptr< Connection > connection ) :
        RawConnection( name, connection ) { }
protected:
    inline bool         Decode( size_t bytes_transferred );
    virtual bool        Handle( const Decoder *decoder ) = 0;
    Decoder             decoder_;
};

template < typename Decoder >
bool RawConnectionImpl< Decoder >::Decode( size_t bytes_transferred ) {
    boost::tribool result;
    const char *start = mBuffer.data();
    const char *end = start + bytes_transferred;
    const char *p = start;
    
    while ( p < end ) {
        boost::tie( result, p ) =
                decoder_.Decode( p, end );
        if ( result ) {
            VLOG( 2 ) << Name() << " : " << "Handle lineformat: size: " << ( p - start );
            if ( !Handle( &decoder_ ) ) {
                return false;
            }
            decoder_.reset();
        } else if ( !result ) {
            VLOG( 2 ) << Name() << " : " << "Parse error";
            return false;
        } else {
            VLOG( 2 ) << Name() << " : " << "Need to read more data";
            return true;
        }
    }
    return true;
}


#endif  /* RAWCONNECTION_HPP */

