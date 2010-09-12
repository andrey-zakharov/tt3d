
// <editor-fold defaultstate="collapsed" desc="functional tests">
#if defined( _WITH_BOOST ) && defined( _DEBUG )
#include <boost/test/unit_test.hpp>
#include "server/Client.hpp"
#include "server/Clients.hpp"
#include "ConcurrentContainer.cpp"
#include "network/Connection.hpp"

using namespace server;

class MockConnection : public Connection {
public:
    MockConnection( const string &name ) : Connection( name ) {};
    virtual void CallMethod(
            const google::protobuf::MethodDescriptor    *method,
            google::protobuf::RpcController             *controller,
            const google::protobuf::Message             *request,
            google::protobuf::Message                   *response,
            google::protobuf::Closure                   *done ) {};
            
    virtual bool RegisterService( google::protobuf::Service *service ) {};

    virtual boost::shared_ptr< Connection > Span( TcpSocket *socket ) { return this->shared_from_this(); };
};
BOOST_AUTO_TEST_CASE( testClients ) {

    Clients clients;
    //ConnectionPtr
    string connName( "ConnTest" );
    MockConnection  *clientConnection = new MockConnection( connName );
    ClientPtr client( new Client( "UserNameTest", boost::shared_ptr< Connection >( clientConnection ) ) );

    int addedID = clients.Add( client );//

    BOOST_CHECK_EQUAL( clients.Size(), 1 );
    BOOST_CHECK( addedID );

    BOOST_CHECK_EQUAL( 0 , clients.Add( client ) );
    BOOST_CHECK_EQUAL( client, clients.Get( addedID ) );

    //same login should be prevented!
    ClientPtr client2(
        new Client(
            "UserNameTest",
            boost::shared_ptr< Connection >( new MockConnection( connName ) ) 
        )
    );

    BOOST_CHECK_EQUAL( 0 , clients.Add( client2 ) );
}



#endif
// </editor-fold>