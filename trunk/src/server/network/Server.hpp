// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#ifndef NET2_SERVER_HPP
#define NET2_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "Containers.hpp"
#include "network/IOServicePool.hpp"
#include "network/Connection.hpp"
#include "thread/Notifier.hpp"
#include "TimerMaster.hpp"

class Connection;

class AcceptorHandler;

/**
 *  The top-level class of the Server.
 */
class Server :
    private boost::noncopyable, public boost::enable_shared_from_this< Server >, public Connection::IAsyncCloseListener {
private:
    static const long kDefaultDrainTimeout = LONG_MAX;
public:
    /// Construct the Server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server(
            int io_service_number,
            int worker_threads,
            long drain_timeout = kDefaultDrainTimeout );
    ~Server();

    void Listen( const string &address, const string &port,
            Connection* connection_template );

    /// Stop the Server.
    void
    Stop();
private:

    struct AcceptorResource {
        TcpAcceptor     *mpAcceptor;
        TcpSocket       **mppSocket;

        AcceptorResource( TcpAcceptor *in_acceptor, TcpSocket **in_socket_pptr ) :
                mpAcceptor( in_acceptor ), mppSocket( in_socket_pptr ) { }

        void Release() {
            mpAcceptor->close();
            delete mpAcceptor;
            ( *mppSocket )->close();
            delete *mppSocket;
            delete mppSocket;
        }
    };
    
    typedef hash_map< string, AcceptorResource > AcceptorTable;
    void ReleaseAcceptor( const string &host );

    void ConnectionClosed( Connection * );

    typedef hash_set< ConnectionPtr > ChannelTable;

    // Handle completion of an asynchronous accept operation.
    void HandleAccept( const boost::system::error_code& e, TcpSocket *socket, Connection *connection_template );

    // The pool of io_service objects used to perform asynchronous operations.
    IOServicePool   io_service_pool_;
    TimerMaster     timer_master_;
    friend class    AcceptorHandler;
    boost::shared_ptr< Notifier > notifier_;
    ChannelTable    channel_table_;
    boost::mutex    channel_table_mutex_;

    AcceptorTable   acceptor_table_;
    boost::mutex    acceptor_table_mutex_;

    boost::shared_mutex     stop_mutex_;
    int                     drain_timeout_;
};
#endif // NET2_SERVER_HPP
