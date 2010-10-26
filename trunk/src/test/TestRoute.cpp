// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#include "Node.hpp"
#include "Node.cpp"
#include "Branch.hpp"
#include "Branch.cpp"
#include "Track.hpp"
#include "Track.cpp"

BOOST_AUTO_TEST_CASE( testNode ) {
    Node node1;
    Node node2;
    MapCoordinate pos( 0, 0, 0 );
    Node node3( pos );
}
BOOST_AUTO_TEST_CASE( testBranch ) {
    NodePtr node1( new Node( MapCoordinate( 0, 0, 0 ) ) );
    NodePtr node2( new Node( MapCoordinate( 1, 0, 0 ) ) );
    Branch branch;
    branch.SetStartNode( node1 );
    branch.SetEndNode( node2 );//no track

    Branch branch2( node1, node2 );

    BOOST_CHECK_EQUAL( branch.Len(), 1 );
    BOOST_CHECK_EQUAL( branch2.Len(), 1 );
}

BOOST_AUTO_TEST_CASE( testRoute ) {
    
}
#endif
// </editor-fold>
