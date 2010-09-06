
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

#include "ServerListener.hpp"
#include "network/Network.hpp"
#define LISTEN_PORT 33000
//TODO
namespace server {

    ServerListener::ServerListener ( IOService &io ) :
        listenSocket( io, TcpEndpoint( boost::asio::ip::tcp::v4(), LISTEN_PORT ) ) //TODO
    {
        //some init
        //Start();
    }


    void
    ServerListener::Start() {
        Connection::mPointer new_connection =
            Connection::create( listenSocket.io_service() );


        //if not sockets disabled (via ENV compiling, etc)
        listenSocket.async_accept(
            new_connection->Socket(),
            boost::bind( &ServerListener::Accept, this, new_connection, boost::asio::placeholders::error )
        );

    }

    void
    ServerListener::Accept( Connection::mPointer new_connection, ///< remember this. todo templating
            const boost::system::error_code& error ) {
        
        if ( !error ) {
            new_connection->Start( );
            Start( );
        }
    }
}

// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE testServerListener

#define SHELL_CHECK_PORTS "netstat -tlnp 2>/dev/null | grep 33000"
//this is functional test, should be under timeout checks

BOOST_AUTO_TEST_CASE( testStart ) {
    IOService io;
    server::ServerListener server( io );
    BOOST_CHECK( 0 != system( SHELL_CHECK_PORTS ) );

    server.Start( ); //< here should socket opens

    BOOST_CHECK( 0 == system( SHELL_CHECK_PORTS ) );

}



#endif
// </editor-fold>

