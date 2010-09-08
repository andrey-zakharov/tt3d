/* 
 * File:   Network.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 0:14
 */

#ifndef NETWORK_HPP
#define	NETWORK_HPP

/**
 * Just typedef aliases file.
 * Also for documentation
 *
 * Client's code just include "Network.hpp"
 */

#include <boost/asio.hpp>
typedef boost::asio::io_service IOService;///< boost::asio::io_service
// more ugly is
//#define IOService boost::asio::io_service
// for -with-io_service???? :)))))
typedef boost::asio::ip::tcp::acceptor TcpAcceptor;

// for more complex just introduse class NetworkLayer ;)
typedef boost::asio::ip::tcp::endpoint TcpEndpoint;

// tcp::v4(); <-> boost::asio...
typedef boost::asio::ip::tcp::socket    TcpSocket;

typedef boost::asio::ip::tcp::resolver  TcpResolver;

#endif	/* NETWORK_HPP */

