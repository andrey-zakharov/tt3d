// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#include "Containers.hpp"
#include "network/ProtobufConnection.hpp"
#include "network/RawProtobufConnection.hpp"
#include "network/Closure.hpp"

static void
CallServiceMethodDone(
        ConnectionPtr               connection,
        const ProtobufDecoder       *decoder,
        boost::shared_ptr< google::protobuf::Message > resource,
        boost::shared_ptr< google::protobuf::Message > response ) {
    
    VLOG( 2 ) << connection->Name() << " : " << "HandleService->CallServiceMethodDone()";
    assert( decoder != NULL );
    const ProtobufLineFormat::MetaData &request_meta = decoder->meta();
    ProtobufLineFormat::MetaData response_meta;
    response_meta.set_type( ProtobufLineFormat::MetaData::RESPONSE );
    response_meta.set_identify( request_meta.response_identify() );

    CHECK( response->AppendToString( response_meta.mutable_content() ) )
            << "Fail to serialize response for requst: ";
    
    EncodeData data = EncodeMessage( &response_meta );

    if ( !connection->PushData( data ) ) {
        delete data.first;
        delete data.second;
    }

    if ( !connection->ScheduleWrite() ) {
        delete data.first;
        delete data.second;
    }
}

static void
HandleService(
        google::protobuf::Service       *service,
        const google::protobuf::MethodDescriptor *method,
        const google::protobuf::Message *request_prototype,
        const google::protobuf::Message *response_prototype,
        const ProtobufDecoder           *decoder,
        ConnectionPtr                   connection ) {
    
    VLOG( 2 ) << connection->Name() << " : " << "HandleService: " << method->full_name();
    const ProtobufLineFormat::MetaData &meta = decoder->meta();
    boost::shared_ptr< google::protobuf::Message > request( request_prototype->New() );
    const string &content = decoder->meta().content();
    VLOG( 2 ) << connection->Name() << " : " << "content size: " << content.size();

    if ( !request->ParseFromArray( content.c_str(), content.size() ) ) {
        LOG( WARNING ) << connection->Name() << " : " << "HandleService but invalid format";
        return;
    }
    
    boost::shared_ptr< google::protobuf::Message > response( response_prototype->New() );
    google::protobuf::Closure *done = NewClosure(
            boost::bind( CallServiceMethodDone,
            connection,
            decoder,
            request,
            response ) );
    service->CallMethod( method, connection.get(), request.get(), response.get(), done );
}

bool
ProtobufConnection::RegisterService( google::protobuf::Service *service ) {

    if ( handler_table_.get() == NULL ) {
        handler_table_.reset( new HandlerTable );
    }

    const google::protobuf::ServiceDescriptor *service_descriptor =
            service->GetDescriptor();

    for ( int i = 0; i < service_descriptor->method_count(); ++i ) {
        const google::protobuf::MethodDescriptor *method = service_descriptor->method( i );
        const google::protobuf::Message *request = &service->GetRequestPrototype( method );
        const google::protobuf::Message *response = &service->GetResponsePrototype( method );
        const string &method_name = method->full_name();
        const uint64 method_fingerprint = hash8( method_name );
        HandlerTable::const_iterator it = handler_table_->find( method_fingerprint );

        CHECK( it == handler_table_->end() )
                << " unfortunately, the method name: " << method_name
                << " is conflict with another name after hash("
                << method_fingerprint << ") please change.";
        handler_table_->insert( make_pair(
                method_fingerprint,
                boost::bind( HandleService, service, method, request, response, _1, _2 ) ) );
    }
}

void
ProtobufConnection::CallMethod(
        const google::protobuf::MethodDescriptor    *method,
        google::protobuf::RpcController             *controller,
        const google::protobuf::Message             *request,
        google::protobuf::Message                   *response,
        google::protobuf::Closure                   *done ) {
    
    RawConnectionStatus::Locker locker( mStatus->Mutex() );
    
    if ( mStatus->IsClosing() || impl_.get() == NULL ) {
        VLOG( 2 ) << "CallMethod " << Name() << " but is closing";
        RpcController *rpc_controller = dynamic_cast< RpcController* > ( controller );

        if ( rpc_controller ) {
            rpc_controller->SetFailed( "Impl Connection is disconnected" );
            rpc_controller->Notify();
        }

        if ( done ) {
            done->Run();
        }
        
        return;
    }
    
    RawProtobufConnection *impl = dynamic_cast< RawProtobufConnection* > ( impl_.get() );
    impl->CallMethod( mStatus, method, controller, request, response, done );
}

bool
ProtobufConnection::Handle( ConnectionPtr connection, const ProtobufDecoder *decoder ) const {

    if ( handler_table_.get() == NULL ) {
        return false;
    }
    
    VLOG( 2 ) << connection->Name() << ".ProtobufConnection.Handle";
    const ProtobufLineFormat::MetaData &meta = decoder->meta();
    HandlerTable::value_type::second_type handler;
    HandlerTable::const_iterator it = handler_table_->find( meta.identify() );

    if ( it != handler_table_->end() ) {
        it->second( decoder, connection );
        return true;
    }
    
    return false;
}

//factory
ConnectionPtr
ProtobufConnection::Span( TcpSocket *socket ) {
    boost::shared_ptr< ProtobufConnection > connection( new ProtobufConnection( Name() + ".span" ) );

    if ( !connection->Attach( this, socket ) ) {
        connection.reset();
    }
    
    return connection;
}

bool
ProtobufConnection::Attach( ProtobufConnection *service_connection, TcpSocket *socket ) {
    static int i = 0;
    const string span_name = this->Name() + boost::lexical_cast< string > ( i++ );

    RawConnection * raw_connection( new RawProtobufConnection(
            span_name,
            shared_from_this(),
            service_connection ) );

    if ( raw_connection == NULL ) {
        LOG( WARNING ) << "Fail to allocate RawProtobufConnection, not enough memory?";
        delete socket;
        return false;
    }

    impl_.reset( raw_connection );
    impl_->InitSocket( mStatus, socket );
    return true;
}
