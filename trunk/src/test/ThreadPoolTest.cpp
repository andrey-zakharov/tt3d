
// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG ) && !defined( testModuleThreadPool )
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE testModuleThreadPool

class ThreadPoolTest {
public:

    void
    Inc( int *cnt ) {
        VLOG( 2 ) << "Inc call, cnt: " << *cnt;
        *cnt = 0xbeef;
    }

};

BOOST_AUTO_TEST_CASE( testThreadPool ) {

    static const int    kPoolSize = 100;
    ThreadPoolTest      testFunctor;
    Logger::Logger( "test" );
    ThreadPool p( "Test", kPoolSize );

    for ( int k = 0; k < 1000; ++k ) {
        p.Start( );
        int item_size = kPoolSize * ( rand( ) % 10 + 1 );
        VLOG( 2 ) << "item size: " << item_size;
        vector< int > v;
        v.resize( item_size, 0 );

        for ( int i = 0; i < item_size; ++i ) {
            boost::function0<void> handler =
                boost::bind( &ThreadPoolTest::Inc, testFunctor, &v[ i ] );
            p.PushTask( handler );
        }

        p.Stop( );

        for ( int i = 0; i < item_size; ++i ) {
            BOOST_CHECK_EQUAL( v[i], 0xbeef );
        }

        VLOG( 0 ) << "thread stopped " << k;
    }

}

#endif // defined( _WITH_BOOST ) && defined( _DEBUG )
// </editor-fold>

