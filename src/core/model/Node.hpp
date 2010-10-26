/* 
 * File:   Node.hpp
 * Author: vaulter
 *
 * Created on 28 Сентябрь 2010 г., 22:28
 */

#ifndef NODE_HPP
#define	NODE_HPP

#include <boost/shared_ptr.hpp>
#include "Coordinate.hpp"
using core::MapCoordinate;
/**
 * Base class for stores, stations.
 */
class Node {
public:
    Node( );
    Node( const MapCoordinate position );
    Node( const Node& orig );
    virtual ~Node( );

    void
    SetPos( MapCoordinate pos ) {
        this->pos = pos;
    }
    
    MapCoordinate
    GetPos( ) const {
        return pos;
    }
private:
    MapCoordinate    pos;
};

typedef boost::shared_ptr< Node >       NodePtr;

#endif	/* NODE_HPP */

