
#include <boost/bind.hpp>
#include "Network.hpp"
#include "NetworkClient.hpp"
#include "network/IOServicePool.hpp"

namespace client {

    NetworkClient::NetworkClient( IOService& io, TcpResolver::iterator endpoint_iterator ) :
    mIo( io ),
    mSocket( io ) {

        TcpEndpoint endpoint = *endpoint_iterator;
        mSocket.async_connect( endpoint,
                boost::bind( &NetworkClient::OnConnect, this,
                boost::asio::placeholders::error, ++endpoint_iterator ) );
    }

    void
    NetworkClient::OnConnect( const boost::system::error_code& error, TcpResolver::iterator endpoint_iterator ) {

        if ( !error ) {
           /* boost::asio::async_read( mSocket,
                    boost::asio::buffer( mReadMessage.data( ), Message::header_length ),
                    boost::bind( &NetworkClient::OnReadHeader, this, boost::asio::placeholders::error ) );*/

        } else if ( endpoint_iterator != TcpResolver::iterator( ) ) {
            mSocket.close( ); // another try
            TcpEndpoint endpoint = *endpoint_iterator;

            mSocket.async_connect( endpoint,
                    boost::bind( &NetworkClient::OnConnect, this,
                    boost::asio::placeholders::error, ++endpoint_iterator ) );
        }
    }

}

#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( testConnection ) {

    IOService    io;

    
}

#endif
