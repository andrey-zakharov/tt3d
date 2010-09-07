/* 
 * File:   Connections.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 12:09
 */

#ifndef CONNECTIONS_HPP
#define	CONNECTIONS_HPP

// manager
/// class Connections -
class Connections {
    // Attributes
public:
    list< ConnectionPtr > connections;
    // Operations
public:
    void Start( ConnectionPtr connection );
    void Stop( ConnectionPtr connection );
    void StopAll( );
};

#endif	/* CONNECTIONS_HPP */

