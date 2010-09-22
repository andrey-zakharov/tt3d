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
        map< string, ViewPortPtr >  viewports; ///<

        time_t                      lastActivity;   ///< Last network activity from client.
        /// This is high-level hack.
        /// should be done by some network connection timeout... 

        Client( const string user_name, ConnectionPtr conn ) :
                login( user_name ),
                connection( conn )
        {};

        Client( const string user_name, const string password_ = "" ) :
                login( user_name ),
                password( password_ )
        {};
        
    protected:
        string          password; ///< User's password. Optional
        ConnectionPtr   connection;//not in use
    };

    typedef shared_ptr< Client >    ClientPtr;

    struct IsUniqueLoginPredicate {//to do abstract
        ClientPtr              target ;

        IsUniqueLoginPredicate( const ClientPtr &client ) :
            target( client ) {};
            
        bool    operator()( const ClientPtr &client ) {
            return client->login == target->login;
        };
    };

}



#endif	/* CLIENT_HPP */

