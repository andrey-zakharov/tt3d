/* 
 * File:   Game.hpp
 * Author: vaulter
 *
 * Created on 12 Сентябрь 2010 г., 16:24
 */

#ifndef GAME_HPP
#define	GAME_HPP

#include "server/Clients.hpp"

namespace base {//TODO move

    template< typename T >
    class Singleton {
    public:

        virtual
        ~Singleton( ) { }

        static T&
        Instance( ) {
            static T object;
            return object;
        }
    };
}
namespace server {
    // consist of world, network server, clients

    struct Game : public base::Singleton< Game > {

        void
        SetClients( ClientsPtr clients ) {
            this->clients = clients;
        }

        ClientsPtr
        GetClients( ) const {
            return clients;
        }

        ClientsPtr clients;

    protected:
        Game() {
            clients.reset( new Clients );

        }
        friend class base::Singleton< Game >;
    };
}

#endif	/* GAME_HPP */

