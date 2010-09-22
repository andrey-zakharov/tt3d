/* 
 * File:   coordinate.hpp
 * Author: vaulter
 *
 * Created on 31 Август 2010 г., 0:23
 */

#ifndef COORDINATE_HPP
#define	COORDINATE_HPP

#include <cmath>
#include <vector>
#include <boost/lambda/loops.hpp>
using namespace boost::lambda; // bad

//http://www.possibility.com/Cpp/CppCodingStandard.html
namespace core {

    /**
     * Common axis names are
     */
    enum Axis {
        AXISX = 0,  ///< We will count an x axis as first coordinate
        AXISY = 1,  ///< y as second
        AXISZ = 2   ///< and sometime z as third for 3d dim space
    };

    /**
     * template of any dimension and any measurement
     * http://stackoverflow.com/questions/692752/naming-conventions-for-template-types
     */
    template < int TDimens, typename TMeasure >
    class Coordinate {
        
        //typedef std::vector< TMeasure >::iterator                  DimsIter;
        
    protected:
        std::vector< TMeasure > value;//,y,z,...

    public:
        
        Coordinate() {}; //very default

        Coordinate( std::vector< TMeasure > const &ix ) { value = ix; }

        Coordinate( TMeasure x, TMeasure y ) { value.push_back( x ); value.push_back( y ); }

        Coordinate( TMeasure x, TMeasure y, TMeasure z ) { value.push_back( x ); value.push_back( y ); value.push_back( z ); }
        
        const       TMeasure    GetX() { return value.at( AXISX ); } // below optimized

        inline 
        const       TMeasure    x() { return value.at( AXISX ); }
        inline
        const       TMeasure    y() { return value.at( AXISY ); }
        inline
        const       TMeasure    z() { return value.at( AXISZ ); }

        //common
        inline
        const       TMeasure    Get( Axis axis ) throw() { return value.at( axis ); }

        Coordinate< TDimens - 1, TMeasure > NormalProjection() {
            std::vector< TMeasure > _v = value;
            _v.pop_back();// very simple transform - throw away Z coords :)
            return Coordinate< TDimens - 1, TMeasure >( _v );
        }

        /**
         * Returns distance to another point
         * General method for any number of dimensions
         * @param second point
         * @return distance
         */
        virtual
        double distance( const Coordinate< TDimens, TMeasure > &_b ) const {

            double ret = 0.0;
            int i = AXISX;
            /*DimIter from = value.begin();

            std::for_each( from,
                (std::vector< TMeasure >::iterator)value.end(),
                ret += pow( double( std::abs( _1 - _b.Get( (Axis)(i++) ) ) ), 2.0 )
            );*/


            ret = pow( ret, 0.5 );

            return ret;
        }

    };
//
//    template < typename TMeasure >
//    class Coordinate< 2, TMeasure > {
//
//    };

    template < typename TMeasure >
    class TwoDCoordinate : public Coordinate< 2, TMeasure > {

    public:

        //upgrading type cast constructor
        TwoDCoordinate( Coordinate< 2, TMeasure > const &copy ) : Coordinate< 2, TMeasure >( copy ) {} ;

        // normal constructor
        TwoDCoordinate( TMeasure x, TMeasure y ) : Coordinate< 2, TMeasure >( x, y ) {} ;

        //double distance( const Coordinate< 2, TMeasure > & _b ) const {

        //optimized specification for 2d
        double distance( const TwoDCoordinate & _b ) const {
            double ret = 0.0 +
                pow( double( std::abs( this->value[ AXISX ] - _b.value[ AXISX ] ) ), 2.0 ) +
                pow( double( std::abs( this->value[ AXISY ] - _b.value[ AXISY ] ) ), 2.0 ) ;

            ret = pow( ret, 0.5 );

            return ret;
        }
    };

    /*in the next life:
    template < typename TMeasure >
    typedef    Coordinate< 2, TMeasure >   TwoDCoordinate;
     */

    template < typename TMeasure >
    class ThreeDCoordinate : public Coordinate< 3, TMeasure > {

    public:
        ThreeDCoordinate() : Coordinate< 3, TMeasure >() {} ;
        ThreeDCoordinate( TMeasure x, TMeasure y, TMeasure z ) : Coordinate< 3, TMeasure >( x, y, z ) {} ;
    };

}


#endif	/* COORDINATE_HPP */

