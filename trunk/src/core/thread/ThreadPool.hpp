// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_
//#include "base/base.hpp"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "Logger.hpp"
#include "Executor.hpp"
#include "PCQueue.hpp"

using std::vector;
using std::string;
using boost::shared_ptr;

class ThreadPool : public boost::noncopyable, public Executor {
public:

    ThreadPool( const string name, int size ) : mName( name ), mSize( size ), mTimeout( kDefaultTimeout ) { }

    ~ThreadPool( ) {
        if ( IsRunning( ) ) {
            LOG( WARNING ) << "Stop thread pool: " << mName << " in destructor";
            Stop( );
            LOG( WARNING ) << "Stopped thread pool: " << mName << " in destructor";
        }
    }

    void
    Start( ) {
        
        boost::mutex::scoped_lock locker( mRunMutex );
        VLOG( 1 ) << Name( ) << " Start, size:" << mSize;

        if ( !mThreads.empty( ) ) {
            VLOG( 1 ) << Name( ) << " Running.";
            return;
        }
        
        for ( int i = 0; i < mSize; ++i ) {
            boost::shared_ptr<boost::thread> t( new boost::thread(
                    boost::bind( &ThreadPool::Loop, this, i, mName.empty( ) ? "NoName" : strdup( mName.c_str( ) ) ) ) );
            mThreads.push_back( t );
        }
    }

    bool
    IsRunning( ) {
        return mThreads.size( ) > 0;
    }

    const string
    Name( ) const {
        return mName;
    }

    void
    SetStopTimeout( int timeout ) {
        mTimeout = timeout;
    }

    int
    Size( ) const {
        return mSize;
    }

    void
    Stop( ) {
        boost::mutex::scoped_lock locker( mRunMutex );
        VLOG( 1 ) << Name( ) << " Stop.";
        
        if ( mThreads.empty( ) ) {
            VLOG( 1 ) << Name( ) << " already stop.";
            return;
        }

        for ( int i = 0; i < mThreads.size( ); ++i ) {
            mPcQueue.Push( boost::function0< void >( ) );
        }
        
        for ( int i = 0; i < mThreads.size( ); ++i ) {
            bool ret = mThreads[ i ]->timed_join( boost::posix_time::seconds( mTimeout ) );
            VLOG( 2 ) << "Join threads: " << i << " ret: " << ret;
        }
        
        VLOG( 1 ) << Name( ) << " Stopped";
        mThreads.clear( );
    }

    void
    PushTask( const boost::function0< void > &t ) {

        if ( t.empty( ) ) {
            LOG( WARNING ) << Name( ) << " can't push null task.";
            return;
        }
        
        mPcQueue.Push( t );
        VLOG( 2 ) << Name( ) << " task pushed.";
    }

    void
    Run( const boost::function0<void> &f ) {
        PushTask( f );
    }
private:
    
    static const int kDefaultTimeout = 60;

    void
    Loop( int i, const char *pool_name ) {
        VLOG( 2 ) << pool_name << " worker " << i << " start.";

        while ( 1 ) {
            VLOG( 2 ) << pool_name << " worker " << i << " wait task.";
            boost::function0<void> h = mPcQueue.Pop( );

            if ( h.empty( ) ) {
                VLOG( 2 ) << pool_name << " worker " << i << " get empty task, so break.";
                break;
            }
            
            VLOG( 2 ) << pool_name << " worker " << i << " running task";
            h( );
            VLOG( 2 ) << pool_name << " worker " << i << " finish task";
        }
        
        VLOG( 2 ) << pool_name << " worker " << i << " stop";
        delete pool_name;
    }
    int                                     mTimeout;
    vector< shared_ptr< boost::thread > >   mThreads;
    int                                     mSize;
    boost::mutex                            mRunMutex;
    PCQueue< boost::function0< void > >     mPcQueue;
    string                                  mName;
};

#endif  // THREAD_POOL_HPP_



