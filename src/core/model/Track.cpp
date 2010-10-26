/* 
 * File:   Track.cpp
 * Author: vaulter
 * 
 * Created on 28 Сентябрь 2010 г., 22:29
 */

#include "Track.hpp"
namespace model {

    Track::Track( ) { }

    Track::Track( const Track& orig ) { }

    Track::~Track( ) { }

    core::lenType
    Track::Len( ) {
        if ( !cachedLen ) {
            //
            cachedLen = 0;
        }

        //TODO by startPoint, endPoint and curve functor calc length
        return cachedLen;
    }
}