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

// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
class IOServicePoolTest {
public:

    void
    Inc( int *cnt ) {
        VLOG( 0 ) << "Inc call, cnt: " << *cnt;
        *cnt = 0xbeef;
    }

    void
    Inc2( int *cnt ) {
        VLOG( 0 ) << "Inc2 call, cnt: " << *cnt;
        sleep( rand( ) % 3 + 1 );
        *cnt = 0xbeef;
    }
protected:
    static const int kPoolSize = 40;
    static const int kThreadSize = 40;

    //tests
public:

    void
    Test1( ) {
        IOServicePool p( "Test", kPoolSize, kThreadSize );
        
        for ( int k = 0; k < 10; ++k ) {
            
            p.Start( );
            BOOST_CHECK( p.mWork.size() == kThreadSize );
            
            int item_size = kPoolSize * ( rand( ) % 10 + 1 );
            VLOG( 2 ) << "item size: " << item_size;
            BOOST_CHECK_GE( item_size, 0 );
            vector< int > v;
            v.resize( item_size, 0 );

            for ( int i = 0; i < item_size; ++i ) {
                
                boost::function0<void> handler = boost::bind(
                        &IOServicePoolTest::Inc, this, &v[ i ] );

                p.GetIOService( ).dispatch( handler );
            }
            
            p.Stop( );
            
            for ( int i = 0; i < item_size; ++i ) {
                BOOST_CHECK_EQUAL( v[ i ], 0xbeef );
            }
            
            VLOG( 0 ) << "pool #" << k << " stopped";
        }
    }

    void
    Test2( ) {
        IOServicePool p( "Test2", kPoolSize, kThreadSize );

        for ( int k = 0; k < 2; ++k ) {
            p.Start( );
            int item_size = kPoolSize * ( rand( ) % 10 + 1 );
            VLOG( 2 ) << "item size: " << item_size;
            vector< int > v;
            v.resize( item_size, 0 );

            for ( int i = 0; i < item_size; ++i ) {
                boost::function0<void> handler = boost::bind(
                        &IOServicePoolTest::Inc2, this, &v[ i ] );
                p.GetIOService( ).post( handler );
            }

            p.Stop( );

            for ( int i = 0; i < item_size; ++i ) {
                BOOST_CHECK_EQUAL( v[i], 0xbeef );
            }
            
            VLOG( 0 ) << "pool #" << k << " stopped";
        }
    }
};

namespace ut = boost::unit_test;
BOOST_AUTO_TEST_CASE( testIOServicePool ) {
    shared_ptr< IOServicePoolTest > instance( new IOServicePoolTest );
    ut::test_suite *ts = BOOST_TEST_SUITE( "IOServicePoolTestSuite" );
    ts->add( BOOST_CLASS_TEST_CASE( &IOServicePoolTest::Test1, instance ) );
    ts->add( BOOST_CLASS_TEST_CASE( &IOServicePoolTest::Test2, instance ) );
    ut::framework::run( ts );
}

#endif
// </editor-fold>

