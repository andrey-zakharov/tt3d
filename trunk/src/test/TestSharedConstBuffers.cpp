// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG ) && !defined( tuSharedConstBuffers )
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE tuSharedConstBuffers

class SharedConstBuffersTest {
public:
    static string
    GetP( const SharedConstBuffers &p ) {
        string ret;

        for ( SharedConstBuffers::const_iterator it = p.begin( ); it != p.end( ); ++it ) {
            ret.append(
                string( boost::asio::buffer_cast< const char * >( *it ), boost::asio::buffer_size( *it ) )
            );
        }

        return ret;
    }

    static const int kPoolSize = 100;
};


BOOST_AUTO_TEST_CASE( testSharedConstBuffers ) {
    SharedConstBuffers p;
    p.push( new string( "hello" ) );
    p.push( new string( "world" ) );
    SharedConstBuffers p1 = p;
    p.consume( 1 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "elloworld" );
    p.consume( 1 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "lloworld" );
    p.consume( 2 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "oworld" );
    p.consume( 2 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "orld" );
    p.consume( 2 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "ld" );
    p.consume( 2 );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p ), "" );
    BOOST_CHECK( p.empty( ) );
    BOOST_CHECK( p.begin( ) == p.end( ) );
    p.clear( );
    BOOST_CHECK_EQUAL( SharedConstBuffersTest::GetP( p1 ), "helloworld" );

}


#endif
// </editor-fold>
