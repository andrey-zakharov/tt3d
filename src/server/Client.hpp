/* 
 * File:   Client.hpp
 * Author: vaulter
 *
 * Created on 11 Сентябрь 2010 г., 20:49
 */

#ifndef CLIENT_HPP
#define	CLIENT_HPP
#include "Common.hpp"
#include "Containers.hpp"
#include "network/Connection.hpp"
#include "ViewPort.hpp"

namespace server {

    struct Client {
    public:
        string                      login;
        map< string, ViewPortPtr >  viewports;


        Client( string user_name, ConnectionPtr conn ) :
                login( user_name ),
                connection( conn )
        {};

        Client( string user_name ) :
                login( user_name )
        {};
        
    protected:
        ConnectionPtr   connection;//not in use
    };

    typedef shared_ptr< Client >    ClientPtr;

    struct IsUniqueLoginPredicate {//to do abstract
        string              target ;
        IsUniqueLoginPredicate( const ClientPtr &client ) :
            target( client->login ) {};
            
        bool    operator()( const ClientPtr &client ) {
            return client->login == target;
        };
    };

}



#endif	/* CLIENT_HPP */

