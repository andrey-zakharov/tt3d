/* 
 * File:   ViewPort.hpp
 * Author: vaulter
 *
 * Created on 12 Сентябрь 2010 г., 0:22
 */

#ifndef VIEWPORT_HPP
#define	VIEWPORT_HPP

namespace server {

    struct ViewPort {
        string name; // < some name for this viewport
        uint64 x; //
        uint64 y;

        enum EViewSize {
            SMALL = 1,
            MEDIUM,
            LARGE
        };

        union {
            EViewSize evs;
            uint64 ivs; //min and max = 100 and min( map.x,map.y )
        } viewsize;

        // unique predicate... is name too .
        // name comes from client as string
        // e.g "Main", "Some Station View", ...
        // TODO

    };

    typedef boost::shared_ptr< ViewPort > ViewPortPtr;
}
#endif	/* VIEWPORT_HPP */

