
// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#include "network/IOServicePool.hpp"
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
    static const int kPoolSize = 2;
    static const int kThreadSize = 4;

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


