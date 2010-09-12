/* 
 * File:   ClientMain.cpp
 * Author: vaulter
 *
 * Created on 19 Август 2010 г., 23:21
 */

#include <cstdlib>
#include "Coordinate.hpp"
#include "network/ClientConnection.hpp"
#include "network/Closure.hpp"
#include "proto/general.pb.h"
//#include "NetworkClient.hpp"
#define server "127.0.0.1"
#define port    "33000"

bool done = false;

class User {
public:

    User() :
        controller( new RpcController )
    { }

    void
    DoRequest() {

        protocol::ClientRequest request;
        boost::shared_ptr< protocol::ClientResponse > response( new protocol::ClientResponse() );
        request.set_login( "test" );

        //network level
        boost::shared_ptr< ClientConnection >
                client_connection( new ClientConnection( "ListenTestMainClient", server, port ) );

        boost::shared_ptr< protocol::clients::Stub >
                client_stub( new protocol::clients::Stub( client_connection.get() ) );

        client_connection->Connect();

        client_stub->Register(
                controller.get(),
                &request,
                response.get(),
                NewClosure( boost::bind( &User::ClientCallDone, this, response ) ) );

    }

    void
    ClientCallDone( boost::shared_ptr< protocol::ClientResponse > response ) {
        LOG( INFO ) << "code: " << response->response().code() << ", id: " << response->client().clientid();
        done = true;
    }
    boost::shared_ptr< RpcController > controller;
};

/**
 *
 */
int
main( int argc, char** argv ) {


    User user;
    user.DoRequest();
    while ( !done ) {
        sleep( 10 );
    }
    return 0;
}
