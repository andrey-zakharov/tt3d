#include <stdlib.h>
#include <error.h>
#include <iostream>
#include <boost/asio.hpp>

#include "lua.hpp"
#include "Network.hpp"
#include "network/ProtobufConnection.cpp"
#include "network/Server.hpp"


//using server::ServerListener;

static int
LS_PrintNumber( lua_State *L ) {
    int argc = lua_gettop( L );

    for ( int n = 1; n <= argc; ++n ) {
        std::cerr << "-- argument " << n << ": "
                << lua_tostring( L, n ) << std::endl;
    }

    // Verify it is a number and print it.
    if ( lua_isnumber( L, 1 ) ) {
        std::cout << lua_tonumber( L, 1 ) << std::endl;
    }

    lua_pushnumber( L, 123 ); // return value
    return 1; // number of return values
}

void testLua();

int
main( int argc, char *argv[] ) {
    std::cout << "Hello! " << std::endl;
    testLua( );

    try {

        boost::shared_ptr< ProtobufConnection >     server_connection_template;
        boost::shared_ptr< Server >                 server( new Server( 2, 4 ) );

        string listen_addr( "0.0.0.0" ), listen_port( "33000" );
        server->Listen( listen_addr, listen_port, server_connection_template.get() );
        

    } catch ( std::exception& e ) {
        std::cerr << e.what( ) << std::endl;
    }

}


void
report_errors( lua_State *L, int status ) {
    if ( status != 0 ) {
        std::cerr << "-- " << lua_tostring( L, -1 ) << std::endl;
        lua_pop( L, 1 ); // remove error message
    }
}

void testLua() {
    //Create state
    lua_State *L;
    L = lua_open();
    luaopen_base( L );
    // make my_function() available to Lua programs
    lua_register( L, "PrintNumber", LS_PrintNumber );

    int status = luaL_loadfile( L, "test.lua" ); //��������� ����

    if ( status ) {
        error( 0, status, "cannot run lua file: %s", lua_tostring(L, -1) );
        //exit( 1 );
        return;
    }

    if ( status == 0 ) {    // execute Lua program
        status = lua_pcall(L, 0, LUA_MULTRET, 0);
    }

    report_errors( L, status );
    
    lua_close(L);
}