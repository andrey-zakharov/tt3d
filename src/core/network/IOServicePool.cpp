// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#include <boost/bind.hpp>
#include "IOServicePool.hpp"
#include "Logger.hpp"

IOServicePool::IOServicePool(   const string    &name,
                                size_t          num_io_services,
                                size_t          num_threads ) :
        mName( name ),
        mNumIOServices( num_io_services ),
        mNumThreads( num_threads ),
        mNextIOService( 0 ),
        mThreadPool( name + ".ThreadPool", mNumThreads ) {

    //CHECK_GE( num_threads, num_io_services );
}

void
IOServicePool::Start( ) {
    boost::mutex::scoped_lock locker( mMutex );

    if ( !mWork.empty( ) ) {
        LOG( WARNING ) << "IOServicePool already running";
        return;
    }
    
    CHECK( mIOServices.empty( ) );

    for ( size_t i = 0; i < mNumIOServices; ++i ) {
        boost::shared_ptr< IOService > io_service( new IOService );
        mIOServices.push_back( io_service );
    }
    
    // Give all the io_services work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    mWork.clear( );
    mWork.reserve( mNumThreads );
    int k = 0;

    for ( int i = 0; i < mNumThreads; ++i ) {
        shared_ptr< IOService> io_service = mIOServices[ k ];
        k = ( k + 1 ) % mNumIOServices;
        
        shared_ptr< IOService::work > worker( new IOService::work( *io_service ) );
        mWork.push_back( worker );
        
        mThreadPool.PushTask( boost::bind( &IOService::run, io_service.get( ) ) );
    }

    mThreadPool.Start( );
    
}

void
IOServicePool::Stop( ) {
    boost::mutex::scoped_lock locker( mMutex );

    if ( mWork.empty( ) ) {
        LOG( WARNING ) << "IOServicePool already stop";
        return;
    }
    // graceful exit
    for ( size_t i = 0; i < mWork.size(); ++i ) {
        mWork[ i ].reset();
    }
    
/* exit with break current jobs
    for ( size_t i = 0; i < mIOServices.size( ); ++i ) {
        mIOServices[ i ]->stop( );
    }*/
    
    mThreadPool.Stop( );
    mWork.clear( );
    mIOServices.clear( );

}

IOService &
IOServicePool::GetIOService( ) {
    boost::mutex::scoped_lock locker( mMutex );
    // Use a round-robin scheme to choose the next io_service to use.
    IOService &io_service = *mIOServices[ mNextIOService ];
    ++mNextIOService;

    if ( mNextIOService == mIOServices.size( ) ) {
        mNextIOService = 0;
    }
    
    return io_service;
}
