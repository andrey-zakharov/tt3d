/* 
 * File:   Clients.hpp
 * Author: vaulter
 *
 * Created on 9 Сентябрь 2010 г., 0:41
 */

#ifndef CLIENTS_HPP
#define	CLIENTS_HPP

#include "proto/general.pb.h"
#include "Game.hpp"
#include "ConcurrentContainer.cpp"

namespace server {

    namespace pb = google::protobuf;
    /// implementation of clients rpc

    class RpcServiceClients : public protocol::clients {
    public:

        //request-response layer.

        void
        Register(
                pb::RpcController *controller,
                const protocol::ClientRequest *request,
                protocol::ClientResponse *response,
                pb::Closure *done
        ) {
            //wait or check for existence of needed containers
            // game->Clients
            //TODO

            //check login for existence on server list
            // if client already in - reject registration
            // else put it to clients list and return his clientID
            protocol::GeneralResponse *general_response = response->mutable_response();
            Game game = Game::Instance(); // instance of game leasing
            // here is place to pre check, but now lets Add works
            general_response->set_code( 0 ); //no error

            ClientPtr new_client( new Client( request->login() ) );//elevate to business level
            // but what about keep connection!? TODO
            
            int userID;

            if ( !( userID = game.GetClients()->Add( new_client ) ) ) {
                general_response->set_code( 1 ); //error
                general_response->set_text( "Failed to add client" );
                general_response->set_close( true );
                LOG( INFO ) << "Failed to add client: " << new_client->login;

            } else {
                protocol::ClientID *client = response->mutable_client();
                client->set_clientid( userID );
                //client->set_ticket(  "some"  );//this is optional field
                LOG( INFO ) << "Client added: " << new_client->login;
            }

            done->Run();
        }

    };

}
#endif	/* CLIENTS_HPP */

