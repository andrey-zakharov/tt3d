// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#include "ClientConnection.hpp"
#include "IOServicePool.hpp"
#include "TimerMaster.hpp"

ClientConnection::ClientConnection(
        const string &name, const string &server, const string &port )
: ProtobufConnection( server + "." + port + "." + name ),
io_service_pool_( new IOServicePool( name + ".IOService", 1, kClientThreadPoolSize ) ),
server_( server ), port_( port ),
timer_master_( new TimerMaster ) {
    VLOG( 2 ) << "Constructor client connection:" << name;
}

ClientConnection::~ClientConnection() {
    CHECK( !IsConnected() );
}

bool
ClientConnection::Connect() {
    if ( IsConnected() ) {
        LOG( WARNING ) << "Connect but IsConnected";
        return true;
    }
    io_service_pool_->Start();
    timer_master_->Start();
    TcpResolver::query query( server_, port_ );
    TcpResolver resolver( io_service_pool_->GetIOService() );
    TcpResolver::iterator endpoint_iterator( resolver.resolve( query ) );
    TcpResolver::iterator end;
    // Try each endpoint until we successfully establish a connection.
    boost::system::error_code error = boost::asio::error::host_not_found;
    TcpSocket *socket = new TcpSocket( io_service_pool_->GetIOService() );

    while ( error && endpoint_iterator != end ) {
        socket->close();
        socket->connect( *endpoint_iterator++, error );
    }
    
    if ( error ) {
        delete socket;
        LOG( WARNING ) << ":fail to connect, error:" << error.message();
        return false;
    }
    
    CHECK( impl_ == NULL );
    Attach( this, socket );
    timer_master_->Register( shared_from_this() );
    return true;
}
