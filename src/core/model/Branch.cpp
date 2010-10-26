/* 
 * File:   Branch.cpp
 * Author: vaulter
 * 
 * Created on 28 Сентябрь 2010 г., 23:13
 */

#include "Common.hpp"
#include "Branch.hpp"

Branch::Branch( ) : cachedLen( 0 ) { }

Branch::Branch( const NodePtr startN, const NodePtr endN ) :
    startNode( startN ), endNode( endN ), cachedLen( 0 ) { }

Branch::Branch( const Branch& orig ) { }

Branch::~Branch( ) { }

lenType
Branch::Len( bool flush ) {

    if ( flush ) {
        cachedLen = 0;
    }

    if ( cachedLen ) {
        return cachedLen;
    }

    if ( tracks.size( ) ) {

        foreach( TrackPtr tr, tracks ) {
            cachedLen += tr->Len( );
        }

    } else {
        cachedLen = endNode->GetPos( ).distance( startNode->GetPos( ) );
    }

    return cachedLen;
}