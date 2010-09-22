
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#define BOOST_TEST_MODULE testTwoDCoordinate

#include <boost/test/unit_test.hpp>
#include "Coordinate.hpp"
// <editor-fold defaultstate="collapsed" desc="as black box">
BOOST_AUTO_TEST_CASE( testConstructor ) {
    core::TwoDCoordinate< int > a( 3, 2 );
    core::TwoDCoordinate< int > b( 2, 3 );

    BOOST_CHECK_EQUAL( a.Get( core::AXISX ), 3 );
    BOOST_CHECK_EQUAL( a.Get( core::AXISY ), 2 );
    BOOST_CHECK_EQUAL( b.Get( core::AXISX ), 2 );
    BOOST_CHECK_EQUAL( b.Get( core::AXISY ), 3 );

    core::ThreeDCoordinate< int > c( 2, 3, 1 );
    BOOST_CHECK_EQUAL( c.Get( core::AXISX ), 2 );
    BOOST_CHECK_EQUAL( c.Get( core::AXISY ), 3 );
    BOOST_CHECK_EQUAL( c.Get( core::AXISZ ), 1 );

}
BOOST_AUTO_TEST_CASE( testProjection ) {
    core::ThreeDCoordinate< int > a( 2, 3, 1 );
    core::TwoDCoordinate< int > b =  a.NormalProjection();

    BOOST_CHECK_EQUAL( b.x(), 2 );
    BOOST_CHECK_EQUAL( b.y(), 3 );

}
// </editor-fold>



#endif
