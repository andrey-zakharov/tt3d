// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#include <sstream>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "network/ClientConnection.hpp"
#include "network/ProtobufConnection.hpp"
#include "network/Server.hpp"
#include "network/Server.cpp"
#include "Logger.hpp"
#include "proto/hello.pb.h"
#include "thread/PCQueue.hpp"
#include "network/Closure.hpp"

#define server  "localhost" //, "The test server");
#define port    "6789" //, "The test server");
#define num_threads     4 //, "The test server thread number");
#define num_connections 4 //, "The test server thread number");

class EchoService2Impl : public Hello::EchoService2 {
public:
public:

    EchoService2Impl( boost::shared_ptr<PCQueue<bool> > pcqueue ) : called_( 0 ), cnt_( 0 ), pcqueue_( pcqueue ) { }

    virtual void
    Echo1( google::protobuf::RpcController *controller,
            const Hello::EchoRequest *request,
            Hello::EchoResponse *response,
            google::protobuf::Closure *done ) {
        LOG( INFO ) << "Echo ServiceImpl called";
        response->set_echoed( 1 );
        response->set_text( "server->" + request->question() );
        response->set_close( false );
        done->Run();
        EchoService2::Stub stub(
                dynamic_cast<google::protobuf::RpcChannel*> ( controller ) );
        Hello::EchoRequest2 *request2 = new Hello::EchoRequest2;
        Hello::EchoResponse2 *response2 = new Hello::EchoResponse2;
        request2->set_question( "server question" + boost::lexical_cast< string > ( i_++ ) );
        boost::shared_ptr<RpcController> controller2( new RpcController );
        google::protobuf::Closure *done2 = NewClosure( boost::bind(
                &EchoService2Impl::CallEcho2Done, this, request2, response2, controller2 ) );
        stub.Echo2( controller2.get(),
                request2,
                response2,
                done2 );
        connection_ = dynamic_cast<Connection*> ( controller );
        VLOG( 2 ) << "CallEcho2 request: " << request2->question();
    }

    virtual void
    Echo2( google::protobuf::RpcController *controller,
            const Hello::EchoRequest2 *request,
            Hello::EchoResponse2 *response,
            google::protobuf::Closure *done ) {
        response->set_echoed( 2 );
        response->set_text( "client->" + request->question() );
        response->set_close( false );
        LOG( INFO ) << "Echo2 called response with:" << response->text();
        done->Run();
    }

    void
    CallEcho2Done( Hello::EchoRequest2 *request,
            Hello::EchoResponse2 *response,
            boost::shared_ptr<RpcController> controller ) {
        static int cnt = 0;
        if ( !response->text().empty() ) {
            CHECK_EQ( "client->" + request->question(), response->text() );
        }
        ++called_;
        LOG( INFO ) << "CallEcho2 response:" << response->text();
        delete response;
        delete request;
        pcqueue_->Push( true );
        LOG( INFO ) << "PCQueue push: " << called_;
    }

    int
    called() const {
        return called_;
    }
private:
    boost::shared_ptr<PCQueue<bool> > pcqueue_;
    Connection *connection_;
    int cnt_;
    int called_;
    int i_;
};


struct ListenTestFixture {

    ListenTestFixture() { //setup
        
        VLOG( 2 ) << "New server connection";
        server_connection_.reset( new ProtobufConnection( "ListenTest.Server" ) );
        server_.reset( new Server( 2, num_threads ) );
        VLOG( 2 ) << "New client connection";
        client_connection_.reset( new ClientConnection( "ListenTestMainClient", server, port ) );
        client_stub_.reset( new Hello::EchoService2::Stub( client_connection_.get() ) );
        pcqueue_.reset( new PCQueue<bool> );
        echo_service_.reset( new EchoService2Impl( pcqueue_ ) );
        VLOG( 2 ) << "Register service";
        server_connection_->RegisterService( echo_service_.get() );
        client_connection_->RegisterService( echo_service_.get() );
        server_->Listen( server, port, server_connection_.get() );
        CHECK( !client_connection_->IsConnected() );
        CHECK( client_connection_->Connect() );
        aborted_ = 0;
        called_ = 0;
    }

    ~ListenTestFixture() {
        VLOG( 2 ) << "Reset server connection";
        server_connection_.reset();
        server_->Stop();
        server_.reset();
        client_stub_.reset();
        VLOG( 2 ) << "Reset client connection";
        client_connection_->Disconnect();
        client_connection_.reset();
        pcqueue_.reset();
        echo_service_.reset();
    }

    boost::shared_ptr< ProtobufConnection >     server_connection_;
    boost::shared_ptr< Server >                 server_;
    boost::shared_ptr< ClientConnection >       client_connection_;
    RpcController                               listener_controller_;
    boost::shared_ptr< Hello::EchoService2::Stub > client_stub_;
    boost::shared_ptr< PCQueue<bool> >          pcqueue_;
    boost::scoped_ptr< EchoService2Impl >       echo_service_;
    int aborted_;
    int called_;

    void
    ClientCallDone( boost::shared_ptr<Hello::EchoResponse> response, boost::shared_ptr<RpcController> controller ) {
        VLOG( 2 ) << "ClientCallDone";
    }

    void
    ClientCallMultiThreadDone(
            boost::shared_ptr<RpcController> controller,
            boost::shared_ptr<Hello::EchoRequest> request,
            boost::shared_ptr<Hello::EchoResponse> response ) {
        VLOG( 2 ) << "CallEcho response: " << response->text();
        VLOG( 2 ) << "ClientCallMultiThreadDone called";
        if ( !controller->Failed() ) {
            CHECK_EQ( "server->" + request->question(), response->text() );
        } else {
            //      CHECK_EQ(controller->ErrorText(), "Abort");
            pcqueue_->Push( true );
            ++aborted_;
            VLOG( 0 ) << "abord: " << aborted_ << " called: " << called_;
        }
        ++called_;
    }

    void
    ClientThreadRun( boost::shared_ptr<Hello::EchoService2::Stub> client_stub ) {
        static int i = 0;
        boost::shared_ptr<Hello::EchoRequest> request( new Hello::EchoRequest );
        boost::shared_ptr<Hello::EchoResponse> response( new Hello::EchoResponse );
        request->set_question( "client question" + boost::lexical_cast<string > ( i++ ) );
        boost::shared_ptr<RpcController> controller( new RpcController );
        client_stub->Echo1( controller.get(),
                request.get(),
                response.get(),
                NewClosure( boost::bind(
                    &ListenTestFixture::ClientCallMultiThreadDone,
                    this, controller, request, response ) )
        );
        VLOG( 2 ) << "CallEcho request: " << request->question();
    }
};

BOOST_FIXTURE_TEST_SUITE( ListenTest, ListenTestFixture )


BOOST_AUTO_TEST_CASE( Test1 ) {
    Hello::EchoRequest request;
    boost::shared_ptr<Hello::EchoResponse> response( new Hello::EchoResponse );
    request.set_question( "client question" );
    boost::shared_ptr<RpcController> controller( new RpcController );
    client_stub_->Echo1( controller.get(),
            &request,
            response.get(),
            NewClosure( boost::bind(
            &ListenTestFixture::ClientCallDone, this, response, controller ) ) );
    pcqueue_->Pop();
    VLOG( 2 ) << "Poped!!";
    client_connection_->Disconnect();
    CHECK_EQ( "server->" + request.question(), response->text() );
    CHECK_EQ( echo_service_->called(), 1 );
}

BOOST_AUTO_TEST_CASE( MultiThreadTest1 ) {
    CHECK( client_connection_->Connect() );
    // Create a pool of threads to run all of the io_services.
    vector<boost::shared_ptr<boost::thread> > threads;
    for ( size_t i = 0; i < num_threads; ++i ) {
        boost::thread t( boost::thread( &ListenTestFixture::ClientThreadRun, this, client_stub_ ) );
    }
    int cnt = 0;
    while ( pcqueue_->Pop() ) {
        ++cnt;
        VLOG( 2 ) << "Get on pop" << cnt;
        if ( cnt == num_threads ) {
            break;
        }
    }
    client_connection_->Disconnect();
    VLOG( 2 ) << "Close client connection";
    CHECK_EQ( aborted_ + echo_service_->called(), num_threads );
}

BOOST_AUTO_TEST_CASE( MultiThreadMultConnectionTest1 ) {
    ThreadPool pool( "Test", num_threads );
    pool.Start();
    boost::shared_ptr<IOServicePool> client_io( new IOServicePool( "TestIO", 2, 4 ) );
    boost::shared_ptr<TimerMaster> tm( new TimerMaster );
    client_io->Start();
    tm->Start();
    vector<boost::shared_ptr< ClientConnection > > connections;
    vector<boost::shared_ptr<Hello::EchoService2::Stub> > stubs;
    
    for ( int i = 0; i < num_connections; ++i ) {
        string name( "MultiThreadMultConnectionTest1Client." + boost::lexical_cast<string > ( i ) );
        boost::shared_ptr< ClientConnection > r( new ClientConnection( name, server, port ) );
        r->RegisterService( echo_service_.get() );
        r->SetIoServicePool( client_io );
        r->SetTimerMaster( tm );
        connections.push_back( r );
        CHECK( r->Connect() );
        boost::shared_ptr<Hello::EchoService2::Stub> s( new Hello::EchoService2::Stub( connections.back().get() ) );
        stubs.push_back( s );

        for ( int j = 0; j < num_threads; ++j ) {
            pool.PushTask( boost::bind( &ListenTestFixture::ClientThreadRun, this, stubs.back() ) );
        }
    }
    
    int cnt = 0;

    while ( pcqueue_->Pop() ) {
        LOG( INFO ) << "Get on pop" << cnt++;
        if ( cnt == num_threads * num_connections ) {
            break;
        }
    }

    for ( int i = 0; i < num_connections; ++i ) {
        connections[ i ]->Disconnect();
    }
    
    pool.Stop();
    client_io->Stop();
    tm->Stop();
    VLOG( 2 ) << "Close client connection";
}

BOOST_AUTO_TEST_SUITE_END()
