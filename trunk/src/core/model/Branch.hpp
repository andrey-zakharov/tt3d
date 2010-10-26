/* 
 * File:   Branch.hpp
 * Author: vaulter
 *
 * Created on 28 Сентябрь 2010 г., 23:13
 */

#ifndef BRANCH_HPP
#define	BRANCH_HPP

#include "Containers.hpp"
#include "Node.hpp"
#include "Track.hpp"

using core::lenType;
using model::TrackPtr;
/**
 * Abstract branch between nodes
 */
class Branch {
public:
    Branch( );
    Branch( const NodePtr startNode, const NodePtr endNode );
    Branch( const Branch& orig );
    virtual ~Branch( );
    void
    SetEndNode( NodePtr endNode ) {
        this->endNode = endNode;
    }
    NodePtr
    GetEndNode( ) const {
        return endNode;
    }
    void
    SetStartNode( NodePtr startNode ) {
        this->startNode = startNode;
    }
    NodePtr
    GetStartNode( ) const {
        return startNode;
    }

    lenType
    Len( bool flush = false );
private:
    NodePtr             startNode;
    NodePtr             endNode;
    list< TrackPtr >    tracks;///< if exists, get length of
    lenType             cachedLen;
    
};

typedef boost::shared_ptr< Branch > BranchPtr;

#endif	/* BRANCH_HPP */

