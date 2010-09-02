#ifndef __common_two_dimensional_coordinate_hpp__
#define __common_two_dimensional_coordinate_hpp__

#include <algorithm>
#include <cmath>

namespace core {

    template < class numeric_t >
    struct TwoDVector {
    public:
        numeric_t m_x;
        numeric_t m_y;

        /**
         * Default constructor initializes to 0, 0
         * This can be removed, because of c++ defaults
         */
        TwoDVector( ) :
            m_x( 0 ),
            m_y( 0 ) { }

        TwoDVector( numeric_t aX, numeric_t aY ) :
            m_x( aX ),
            m_y( aY ) { }

        void add( TwoDVector const & _b ) {
            m_x += _b.m_x;
            m_y += _b.m_y;
        }

        void sub( TwoDVector const & _b ) {
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
        double distance( const TwoDVector & _b ) const {
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
    TwoDVector< numeric_t > operator%(
    TwoDVector< numeric_t > const & _a,
    numeric_t _b
    ) {
        TwoDVector< numeric_t > ret( _a );
        ret.m_x = _a.m_x % _b;
        ret.m_y = _a.m_y % _b;
        return ret;
    }


    template< class numeric_t >
    inline
    TwoDVector< numeric_t > operator%(
        TwoDVector< numeric_t > const & _a,
        TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_x % _b.m_x;
        ret.m_y = _a.m_y % _b.m_y;
        return ret;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t > operator%=(
        TwoDVector< numeric_t > & _a,
        numeric_t _b
    ) {
        _a.m_x = _a.m_x % _b;
        _a.m_y = _a.m_y % _b;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t > operator%=(
        TwoDVector< numeric_t > & _a,
        TwoDVector< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x % _b.m_x;
        _a.m_y = _a.m_y % _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator+(
    TwoDVector< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDVector< numeric_t > ret( _a );
        ret.m_x = _a.m_x + _b;
        ret.m_y = _a.m_y + _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator+(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_x + _b.m_x;
        ret.m_y = _a.m_y + _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator+=(
    TwoDVector< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x + _b;
        _a.m_y = _a.m_y + _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator+=(
    TwoDVector< numeric_t > & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x + _b.m_x;
        _a.m_y = _a.m_y + _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator-(
    TwoDVector< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDVector< numeric_t > ret( _a );
        ret.m_x = _a.m_x - _b;
        ret.m_y = _a.m_y - _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator-(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_x - _b.m_x;
        ret.m_y = _a.m_y - _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator-=(
    TwoDVector< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x - _b;
        _a.m_y = _a.m_y - _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator-=(
    TwoDVector< numeric_t > & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x - _b.m_x;
        _a.m_y = _a.m_y - _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator*(
    TwoDVector< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDVector< numeric_t > ret( _a );
        ret.m_x = _a.m_x * _b;
        ret.m_y = _a.m_y * _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator*(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_x * _b.m_x;
        ret.m_y = _a.m_y * _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator*=(
    TwoDVector< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x * _b;
        _a.m_y = _a.m_y * _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator*=(
    TwoDVector< numeric_t > & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x * _b.m_x;
        _a.m_y = _a.m_y * _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator/(
    TwoDVector< numeric_t > const & _a
    , numeric_t _b
    ) {
        TwoDVector< numeric_t > ret( _a );
        ret.m_x = _a.m_x / _b;
        ret.m_y = _a.m_y / _b;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator/(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_x / _b.m_x;
        ret.m_y = _a.m_y / _b.m_y;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator/=(
    TwoDVector< numeric_t > & _a
    , numeric_t _b
    ) {
        _a.m_x = _a.m_x / _b;
        _a.m_y = _a.m_y / _b;
        return _a;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    operator/=(
    TwoDVector< numeric_t > & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        _a.m_x = _a.m_x / _b.m_x;
        _a.m_y = _a.m_y / _b.m_y;
        return _a;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t > switch_values( TwoDVector< numeric_t > const & _a ) {
        TwoDVector< numeric_t > ret;
        ret.m_x = _a.m_y;
        ret.m_y = _a.m_x;
        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t >
    switch_values( TwoDVector< numeric_t > & _a ) {
        std::swap< numeric_t > ( _a.m_x, _a.m_y );
    }


    template< class numeric_t >
    inline
    bool
    operator==(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        return _a.m_x == _b.m_x && _a.m_y == _b.m_y;
    }

    template< class numeric_t >
    inline
    bool
    operator!=(
    TwoDVector< numeric_t > const & _a
    , TwoDVector< numeric_t > const & _b
    ) {
        return _a.m_x != _b.m_x || _a.m_y != _b.m_y;
    }

    template< class numeric_t >
    inline
    TwoDVector< numeric_t > common_part(
        TwoDVector< numeric_t > const & _a,
        TwoDVector< numeric_t > const & _b
    ) {
        TwoDVector< numeric_t > first;
        TwoDVector< numeric_t > second;

        if ( _a.m_x < _b.m_x ) {
            first = _a;
            second = _b;
        } else {
            first = _b;
            second = _a;
        }

        TwoDVector< numeric_t > ret( 0, 0 );

        if ( first.m_y < second.m_x ) return ret;

        ret.m_x = second.m_x;

        ret.m_y = std::min( first.m_y, second.m_y );

        return ret;
    }
    

    template< class numeric_t >
    inline
    TwoDVector< numeric_t > floor(
        TwoDVector< numeric_t > _source ) {
        TwoDVector< numeric_t > return_value;

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
    numeric_t operator<(
        TwoDVector< numeric_t > const & _left,
        TwoDVector< numeric_t > const & _right ) {
        
        if ( _left.m_x < _right.m_x )       return true;
        else if ( _left.m_x > _right.m_x )  return false;
        return _left.m_y < _right.m_y;
    }
    

    template< typename numeric_1_t, typename numeric_2_t >
    inline
    TwoDVector< numeric_2_t > convert(
        TwoDVector< numeric_1_t > const & src ) {
        
        return TwoDVector< numeric_2_t > ( numeric_2_t( src.m_x ), numeric_2_t( src.m_y ) );
    }
    // </editor-fold>
};

#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE testTwoDimensionalCoordinate

// <editor-fold defaultstate="collapsed" desc="as black box">
BOOST_AUTO_TEST_CASE( testCommonPart ) {
    core::TwoDVector< int > a( 3, 2 );
    core::TwoDVector< int > b( 2, 3 );

    BOOST_CHECK_EQUAL( core::common_part( a, b ).m_x, 2 );
    BOOST_CHECK_EQUAL( core::common_part( a, b ).m_y, 2 );

    a.m_x = 5;    a.m_y = 0;
    b.m_x = 0;    b.m_y = 5;

    BOOST_CHECK_EQUAL( core::common_part( a, b ).m_x, 0 );
    BOOST_CHECK_EQUAL( core::common_part( a, b ).m_y, 0 );

}

BOOST_AUTO_TEST_CASE( testDIstance ) {
        core::TwoDVector< int > a( 3, 4 ); ///< distance = 5
        BOOST_CHECK_EQUAL( a.distance( core::TwoDVector< int > (0, 0) ), 5 );
}
// </editor-fold>


#endif
#endif