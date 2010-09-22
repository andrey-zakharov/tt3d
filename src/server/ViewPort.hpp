/* 
 * File:   ViewPort.hpp
 * Author: vaulter
 *
 * Created on 12 Сентябрь 2010 г., 0:22
 */

#ifndef VIEWPORT_HPP
#define	VIEWPORT_HPP

#include "Coordinate.hpp"
using core::ThreeDCoordinate;

namespace server {

    /**
     * This is viewport of MAP, not just a window.
     * It has one camera on render side. but here store
     * only direction of it
     */
    struct ViewPort {
        string                  name;       /// < Name for this viewport. Unique for each client
        ThreeDCoordinate< int > pos;        /// < Current position of
        ThreeDCoordinate< int > direction;  /// < Current direction to

        enum EViewSize {
            SMALL = 1,
            MEDIUM,
            LARGE
        };

        union {
            EViewSize   evs;
            uint32         ivs; //min and max = 100 and min( map.x,map.y )
        }       viewsize;

        // unique predicate... is name too .
        // name comes from client as string
        // e.g "Main", "Some Station View", ...
        // TODO

    };

    typedef boost::shared_ptr< ViewPort > ViewPortPtr;
}
#endif	/* VIEWPORT_HPP */

