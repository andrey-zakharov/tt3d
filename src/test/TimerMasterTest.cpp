// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)
// modifier: andrey.al.zakharov@gmail.com

#include <boost/test/unit_test.hpp>
#include <sstream>
#include "Logger.hpp"
#include "Timer.hpp"
#include "TimerMaster.hpp"


struct TestTimer : public Timer, public boost::enable_shared_from_this< TestTimer > {

    bool Period( ) const {
        return period_;
    }

    int Timeout( ) const {
        return timeout_;
    }

    void Expired( ) {
        expired_ = true;
    }
    bool period_, expired_;
    int timeout_;
};

BOOST_AUTO_TEST_SUITE( TimerMasterTest )

BOOST_AUTO_TEST_CASE( Test0 ) {
    boost::shared_ptr< TestTimer > timer( new TestTimer );
    TimerMaster m;
    timer->period_ = true;
    timer->timeout_ = 2;
    timer->expired_ = false;
    m.Register( timer );
    m.Update( 2 );
    BOOST_CHECK( !timer->expired_ );
    m.Update( 3 );
    BOOST_CHECK( timer->expired_ );
}

BOOST_AUTO_TEST_CASE( Test1 ) {
    boost::shared_ptr< TestTimer > timer( new TestTimer );
    TimerMaster m;
    timer->period_ = true;
    timer->timeout_ = 1;
    timer->expired_ = false;
    m.Register( timer );

    for ( int i = 2; i < 0xFFFF; ++i ) {
        m.Update( i );
        BOOST_CHECK_MESSAGE( timer->expired_ , i );
        timer->expired_ = false;
    }
    
    for ( int i = 0xFFFF; i < 0xFFFFF; ++i ) {
        m.Update( i );
        BOOST_CHECK( timer->expired_ );
        timer->expired_ = false;
    }
}

BOOST_AUTO_TEST_CASE( Test2 ) {
    vector< boost::shared_ptr< TestTimer > > timers;
    TimerMaster m;

    for ( int i = 2; i < 0x10000; ++i ) {
        boost::shared_ptr< TestTimer > timer( new TestTimer );
        timer->period_ = false;
        timer->timeout_ = i - 1;
        timer->expired_ = false;
        m.Register( timer );
        timers.push_back( timer );
    }

    for ( int i = 2; i < 0x10000; ++i ) {
        m.Update( i );
        BOOST_CHECK( timers[i - 2]->expired_ );
        timers[i - 2]->expired_ = false;
        VLOG( 2 ) << i;
    }
    
    BOOST_CHECK( ! timers[ 0 ]->expired_ );
}

BOOST_AUTO_TEST_CASE( Test3 ) {
    TimerMaster m;

    for ( int i = 1; i < 0x10000; ++i ) {
        boost::shared_ptr<TestTimer> timer( new TestTimer );
        timer->period_ = false;
        timer->timeout_ = i;
        timer->expired_ = false;
        m.Register( timer );
    }
    
    for ( int i = 1; i < 0x10000; ++i ) {
        m.Update( i );
    }
}

BOOST_AUTO_TEST_CASE( Test4 ) {
    static const int magic[] = {
        1,
        2,
        5,
        255,
        254,
        256,
        0xFF00,
        0x1000,
        0xFF01,
        0xFFFF
    };
    int max_timeout = 0xFFFF;
    srand( time( 0 ) );
    
    for ( int i = 0; i < arraysize( magic ); ++i ) {
        TimerMaster m;
        boost::shared_ptr<TestTimer> timer( new TestTimer );
        timer->period_ = true;
        timer->timeout_ = magic[i];
        timer->expired_ = false;
        m.Register( timer );
        m.Update( magic[i] );
        BOOST_CHECK_MESSAGE( ! timer->expired_, magic[ i ] );
        int t = magic[i] + rand( ) % 0xFFFFF + 1;
        LOG( INFO ) << "Update: " << t;
        m.Update( t );
        LOG( INFO ) << "Pass: " << t;
        BOOST_CHECK_MESSAGE( timer->expired_ , t );
    }
}

BOOST_AUTO_TEST_CASE( Test5 ) {
    static const int magic[] = {
        1,
        2,
        5,
        255,
        254,
        256,
        0xFF00,
        0x1000,
        0xFF01,
        0xFFFF
    };
    int max_timeout = 0xFFFF;
    vector<boost::shared_ptr<TestTimer> > timers;
    TimerMaster m;
    for ( int i = 0; i < arraysize( magic ); ++i ) {
        boost::shared_ptr<TestTimer> timer( new TestTimer );
        timer->period_ = true;
        timer->timeout_ = magic[i];
        timer->expired_ = false;
        m.Register( timer );
        timers.push_back( timer );
    }
    
    for ( int i = 2; i < max_timeout; ++i ) {
        m.Update( i );
        if ( i > 0 ) {
            for ( int j = 0; j < timers.size( ); ++j ) {
                if ( ( i - 1 ) % timers[j]->timeout_ == 0 ) {
                    BOOST_CHECK_MESSAGE( timers[j]->expired_, j );
                    timers[j]->expired_ = false;
                }
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()