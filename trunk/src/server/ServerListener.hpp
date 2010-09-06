/* 
 * File:   ServerListener.hpp
 * Author: vaulter
 *
 * Created on 3 Сентябрь 2010 г., 23:53
 */

#ifndef SERVERLISTENER_HPP
#define	SERVERLISTENER_HPP

#include "Containers.hpp"
#include "Network.hpp"
#include "Connection.hpp"

namespace server {
  // thread
    //class ServerOptions;

  class ServerListener {
    // Associations
    List< ConnectionPtr > connectedUsers;
    // Attributes
  public:
    //ServerOptions   options;
    TcpAcceptor     listenSocket;
    //for pre init - endpoint
    //TcpEndpoint     listenEndpoint;
    // Operations
  public:
      
    ServerListener( IOService & );
    /// Loads config
    bool Config ();
    /// Starts listen
    void Start();
    /// Accept user tcp connection
    void Accept( Connection::mPointer, const boost::system::error_code& error ); // god damn!
    /// Thread entry
    void Run ();


  };

}

#endif	/* SERVERLISTENER_HPP */

