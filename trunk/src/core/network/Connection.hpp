// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#ifndef NET2_CONNECTION_HPP_
#define NET2_CONNECTION_HPP_

#include <boost/thread.hpp>
//#include <boost/signals2/signal.hpp>
#include <protobuf/message.h>
#include <protobuf/descriptor.h>
#include <protobuf/service.h>

#include "Logger.hpp"
#include "proto/meta.pb.h"

#include "SharedConstBuffers.hpp"
#include "RawConnection.hpp"

#include "thread/Notifier.hpp"
#include "Timer.hpp"

using std::string;
using std::vector;
using boost::shared_ptr;

class RpcController : virtual public google::protobuf::RpcController {
public:

    RpcController( const string name = "NoNameRpcController" ) : mNotifier( new Notifier( name ) ) { }

    void
    Reset() {
        mstrFailed.clear();
        mNotifier.reset( new Notifier( "RPCController" ) );
    }

    void
    SetFailed( const string &failed ) {
        mstrFailed = failed;
    }

    bool
    Failed() const {
        return !mstrFailed.empty();
    }

    string
    ErrorText() const {
        return mstrFailed;
    }

    void
    StartCancel() { }

    bool
    IsCanceled() const {
        return false;
    }

    void
    NotifyOnCancel( google::protobuf::Closure *callback ) { }

    bool
    Wait() {
        return mNotifier->Wait();
    }

    bool
    Wait( int timeout_ms ) {
        return mNotifier->Wait( timeout_ms );
    }

    void
    Notify() {
        mNotifier->Notify();
    }
private:
    string                          mstrFailed;
    bool                            mbResponsed;
    boost::shared_ptr< Notifier >   mNotifier;
};

class Connection : virtual public RpcController, virtual public google::protobuf::RpcChannel,
        virtual public Timer, public boost::enable_shared_from_this< Connection > {
private:
    static const int kTimeoutSec = 30;
public:

    class IAsyncCloseListener {
    public:
        virtual void ConnectionClosed( Connection * ) = 0;
    };

    Connection( const string &name ) :
        RpcController( name ),
        mName( name ),
        mId( ++globalConnectionId ),
        mStatus( new RawConnectionStatus ) { }

    virtual bool RegisterService( google::protobuf::Service *service ) = 0;

    virtual void CallMethod(
            const google::protobuf::MethodDescriptor    *method,
            google::protobuf::RpcController             *controller,
            const google::protobuf::Message             *request,
            google::protobuf::Message                   *response,
            google::protobuf::Closure                   *done ) = 0;

    bool
    RegisterAsyncCloseListener( boost::weak_ptr< IAsyncCloseListener > listener ) {
        boost::mutex::scoped_lock locker( mListenerMutex );
        mListeners.push_back( listener );
        return true;
    }

    virtual void
    Disconnect() {
        if ( mStatus->IsClosing() ) {
            VLOG( 2 ) << "Disconnect " << Name() << " but is closing";
            return;
        }
        VLOG( 2 ) << "Disconnect " << Name();
        mStatus->Mutex().lock_shared();
        if ( impl_ ) {
            impl_->Disconnect( mStatus, false );
            return;
        }
        mStatus->Mutex().unlock_shared();
        VLOG( 2 ) << "Disconnected " << Name();
    }

    virtual bool
    IsConnected() const {
        return !mStatus->IsIdle() && !mStatus->IsClosing();
    }

    const string
    Name() const {
        return impl_.get() ? impl_->Name() : mName;
    }

    virtual
    ~Connection() {
        CHECK( !IsConnected() );
    }

    virtual int
    Timeout() const {
        return kTimeoutSec;
    }

    virtual bool
    Period() const {
        return IsConnected();
    }

    virtual void
    Expired() {
        VLOG( 2 ) << Name() << " Expired";
        mStatus->Mutex().lock_shared();

        if ( mStatus->IsClosing() ) {
            VLOG( 2 ) << "Heartbeat " << Name() << " but is closing";
            mStatus->Mutex().unlock_shared();
            return;
        }

        if ( impl_.get() == NULL ) {
            mStatus->Mutex().unlock_shared();
            return;
        }
        
        impl_->Heartbeat( mStatus );
    }

    inline bool
    ScheduleWrite() {
        RawConnectionStatus::Locker locker( mStatus->Mutex() );
        if ( mStatus->IsClosing() ) {
            VLOG( 2 ) << "ScheduleWrite " << Name() << " but is closing";
            return false;
        }
        return impl_.get() && impl_->ScheduleWrite( mStatus );
    }

    template < typename T >
    // The push will take the ownership of the data
    inline bool
    PushData( const T &data ) {
        RawConnectionStatus::Locker locker( mStatus->Mutex() );
        if ( mStatus->IsClosing() ) {
            VLOG( 2 ) << "PushData " << Name() << " but is closing";
            return false;
        }
        return impl_.get() && impl_->PushData( data );
    }
    // Create a connection from a socket.
    // The protocol special class should implement it.
    virtual boost::shared_ptr< Connection > Span( TcpSocket *socket ) = 0;
protected:
    static int globalConnectionId;

    void
    ImplClosed() {
        VLOG( 2 ) << mName << "ImplClosed";
        boost::mutex::scoped_lock locker( mListenerMutex );
        
        for ( int i = 0; i < mListeners.size(); ++i ) {
            VLOG( 2 ) << "listener " << i << " expired: " << mListeners[i].expired();
            boost::shared_ptr<IAsyncCloseListener> listener = mListeners[i].lock();

            if ( !mListeners[i].expired() ) {
                listener->ConnectionClosed( this );
            }
        }
        
        mListeners.clear();
        impl_.reset();
    }
    
    boost::intrusive_ptr< RawConnectionStatus > mStatus;
    string                                      mName;
    boost::scoped_ptr< RawConnection >          impl_;
    int                                         mId;
    vector< boost::weak_ptr< IAsyncCloseListener > > mListeners;
    boost::mutex                                mListenerMutex;
    friend class RawConnection;
};

typedef boost::shared_ptr< Connection >        ConnectionPtr;
#endif // NET2_CONNECTION_HPP_
