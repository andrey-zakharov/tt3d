/* 
 * File:   Clients.hpp
 * Author: vaulter
 *
 * Created on 11 Сентябрь 2010 г., 20:47
 */

#ifndef SERVER_CLIENTS_HPP
#define	SERVER_CLIENTS_HPP

#include "Common.hpp"
#include "Containers.hpp"
#include "ConcurrentContainer.hpp"
#include "Client.hpp"


namespace server {

    class Clients : public base::ConcurrentContainer< vector< ClientPtr >, IsUniqueLoginPredicate > {
    public:


        void
        Remove( ClientPtr client );


    protected:
        //vector< server::ClientPtr >  mClients;
        
    private:
        //boost::mutex            mClientsListMutex;
        //CT                      *mpNextClientID;           ///<next client will get this hash
    };

    typedef boost::shared_ptr< Clients >    ClientsPtr;

}
#endif	/* SERVER_CLIENTS_HPP */

