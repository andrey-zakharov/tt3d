#ifndef __common_two_dimensional_coordinate_hpp__
#define __common_two_dimensional_coordinate_hpp__

#include <algorithm>
#include <cmath>

namespace core {

    template < class numeric_t >
    struct TwoDimensionalCoordinate {
    public:
        numeric_t m_x;
        numeric_t m_y;

        /**
         * Default constructor initializes to 0, 0
         * This can be removed, because of c++ defaults
         */
        TwoDimensionalCoordinate( ) :
            m_x( 0 ),
            m_y( 0 ) { }

        TwoDimensionalCoordinate( numeric_t aX, numeric_t aY ) :
            m_x( aX ),
            m_y( aY ) { }

        void add( TwoDimensionalCoordinate const & _b ) {
            m_x += _b.m_x;
            m_y += _b.m_y;
        }

        void sub( TwoDimensionalCoordinate const & _b ) {
            m_x -= _b.m_x;
            m_y -= _b.m_y;
        }

        void mul( numeric_t _val ) {
            m_x *= _val;
            m_y *= _val;
        }

        /**
         * Returns distance to another point
         * @param second point
         * @return distance
         */
        double distance( const TwoDimensionalCoordinate & _b ) const {
            double ret = 0.0 +
                pow( double( std::abs( m_x - _b.m_x ) ), 2.0 ) +
                pow( double( std::abs( m_y - _b.m_y ) ), 2.0 ) ;

            ret = pow( ret, 0.5 );

            return ret;
        }
    };
    
    // <editor-fold defaultstate="collapsed" desc="Operators and helpers">

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > operator%(
    TwoDimensionalCoordinate< numeric_t > const & _a,
    numeric_t _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret( _a );
        ret.m_x = _a.m_x % _b;
        ret.m_y = _a.m_y % _b;
        return ret;
    }


    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > operator%(
        TwoDimensionalCoordinate< numeric_t > const & _a,
        TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_x % _b.m_x;
        ret.m_y = _a.m_y % _b.m_y;
        return ret;
    }

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > operator%=(
        TwoDimensionalCoordinate< numeric_t > & _a,
        numeric_t _b
    ) {
        _a.m_x = _a.m_x % _b;
        _a.m_y = _a.m_y % _b;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > operator%=(
        TwoDimensionalCoordinate< numeric_t > & _a,
        TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x % _b.m_x;
        _a.m_y = _a.m_y % _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator+(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret( _a );
        ret.m_x = _a.m_x + _b;
        ret.m_y = _a.m_y + _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator+(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_x + _b.m_x;
        ret.m_y = _a.m_y + _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator+=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x + _b;
        _a.m_y = _a.m_y + _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator+=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x + _b.m_x;
        _a.m_y = _a.m_y + _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator-(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret( _a );
        ret.m_x = _a.m_x - _b;
        ret.m_y = _a.m_y - _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator-(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_x - _b.m_x;
        ret.m_y = _a.m_y - _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator-=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x - _b;
        _a.m_y = _a.m_y - _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator-=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x - _b.m_x;
        _a.m_y = _a.m_y - _b.m_y;
        return _a;
    }
    //#############################################################################

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator*(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret( _a );
        ret.m_x = _a.m_x * _b;
        ret.m_y = _a.m_y * _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator*(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_x * _b.m_x;
        ret.m_y = _a.m_y * _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator*=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x * _b;
        _a.m_y = _a.m_y * _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator*=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x * _b.m_x;
        _a.m_y = _a.m_y * _b.m_y;
        return _a;
    }
    //#############################################################################

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator/(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret( _a );
        ret.m_x = _a.m_x / _b;
        ret.m_y = _a.m_y / _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator/(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_x / _b.m_x;
        ret.m_y = _a.m_y / _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator/=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x / _b;
        _a.m_y = _a.m_y / _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    operator/=(
    TwoDimensionalCoordinate< numeric_t > & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x / _b.m_x;
        _a.m_y = _a.m_y / _b.m_y;
        return _a;
    }
    //#############################################################################

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > switch_values( TwoDimensionalCoordinate< numeric_t > const & _a ) {
        TwoDimensionalCoordinate< numeric_t > ret;
        ret.m_x = _a.m_y;
        ret.m_y = _a.m_x;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t >
    switch_values( TwoDimensionalCoordinate< numeric_t > & _a ) {
        std::swap< numeric_t > ( _a.m_x, _a.m_y );
    }


    template< class numeric_t >
    inline
    bool
    operator==(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        return _a.m_x == _b.m_x && _a.m_y == _b.m_y;
    }

    template< class numeric_t >
    inline
    bool
    operator!=(
    TwoDimensionalCoordinate< numeric_t > const & _a
    , TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        return _a.m_x != _b.m_x || _a.m_y != _b.m_y;
    }

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > common_part(
        TwoDimensionalCoordinate< numeric_t > const & _a,
        TwoDimensionalCoordinate< numeric_t > const & _b
    ) {
        TwoDimensionalCoordinate< numeric_t > first;
        TwoDimensionalCoordinate< numeric_t > second;

        if ( _a.m_x < _b.m_x ) {
            first = _a;
            second = _b;
        } else {
            first = _b;
            second = _a;
        }

        TwoDimensionalCoordinate< numeric_t > ret( 0, 0 );

        if ( first.m_y < second.m_x ) return ret;

        ret.m_x = second.m_x;

        ret.m_y = std::min( first.m_y, second.m_y );

        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDimensionalCoordinate< numeric_t > floor(
        TwoDimensionalCoordinate< numeric_t > _source ) {
        TwoDimensionalCoordinate< numeric_t > return_value;

        return_value.m_x = std::floor( _source.m_x );
        return_value.m_y = std::floor( _source.m_y );

        return return_value;
    }
    

    template< class numeric_t >
    inline
    numeric_t distance( numeric_t _longitunal, numeric_t _latitunal ) {
        numeric_t temporary_1 = _longitunal * _longitunal;
        numeric_t temporary_2 = _latitunal * _latitunal;

        return std::pow( temporary_1 + temporary_2, 0.5 );
    }
    

    template< class numeric_t >
    inline
    numeric_t
    operator<(
    TwoDimensionalCoordinate< numeric_t > const & _left
    , TwoDimensionalCoordinate< numeric_t > const & _right
    ) {
        if ( _left.m_x < _right.m_x )
            return true;
        else if ( _left.m_x > _right.m_x )
            return false;
        return _left.m_y < _right.m_y;
    }
    

    template< typename numeric_1_t, typename numeric_2_t >
    inline
    TwoDimensionalCoordinate< numeric_2_t > convert(
        TwoDimensionalCoordinate< numeric_1_t > const & src ) {
        
        return TwoDimensionalCoordinate< numeric_2_t > ( numeric_2_t( src.m_x ), numeric_2_t( src.m_y ) );
    }
    // </editor-fold>
};

#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE testTwoDimensionalCoordinate

BOOST_AUTO_TEST_CASE(testCommonPart)
{
    core::TwoDimensionalCoordinate< int > a( 3, 2 );
    core::TwoDimensionalCoordinate< int > b( 2, 3 );

    BOOST_CHECK_EQUAL( core::common_part( a, b ).m_x, 2 );
}

#endif
#endif