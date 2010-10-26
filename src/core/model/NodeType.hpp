/* 
 * File:   NodeType.hpp
 * Author: vaulter
 *
 * Created on 13 Октябрь 2010 г., 0:13
 */

#ifndef NODETYPE_HPP
#define	NODETYPE_HPP
#include <list>
#include "CargoType.hpp"
class NodeType {
public:
    NodeType( );
    NodeType( const NodeType& orig );
    virtual ~NodeType( );
private:
    //TODO bitmask<
    std::list< model::CargoTypeID > served, consume, emit;

};

#endif	/* NODETYPE_HPP */

