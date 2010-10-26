/* 
 * File:   Track.hpp
 * Author: vaulter
 *
 * Created on 28 Сентябрь 2010 г., 22:29
 */

#ifndef TRACK_HPP
#define	TRACK_HPP
#include <boost/shared_ptr.hpp>
#include "Coordinate.hpp"
namespace model {

    /**
     * Straight or curved road or rail or tram track
     * can be with basement, or not (inside another basement also, as road crossings)
     * can be crossing.
     */
    class Track {
    public:
        Track( );
        Track( const Track& orig );
        virtual ~Track( );

        core::lenType
        Len( );

    private:
        core::lenType cachedLen;

    };

    typedef boost::shared_ptr< Track >  TrackPtr;
}
#endif	/* TRACK_HPP */

