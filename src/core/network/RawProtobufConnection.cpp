// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)


#include "Containers.hpp"
#include "Connection.hpp"
#include "RawConnection.hpp"
#include "ProtobufConnection.hpp"
#include "RawProtobufConnection.hpp"
#include "Closure.hpp"


#define RawConnTrace VLOG(2) << Name() << ".protobuf : " << __func__ << " "

RawProtobufConnection::~RawProtobufConnection() {
    VLOG( 2 ) << Name() << " : " << "Destroy protobuf connection";
    ReleaseResponseTable();
}

void
RawProtobufConnection::ReleaseResponseTable() {
    vector<HandlerFunctor> handlers;
    {
        boost::mutex::scoped_lock locker( mResponseHandlerTableMutex );

        for ( HandlerTable::iterator it = mResponseHandlerTable.begin(); it != mResponseHandlerTable.end(); ++it ) {
            handlers.push_back( it->second );
        }

        mResponseHandlerTable.clear();
    }

    for ( int i = 0; i < handlers.size(); ++i ) {
        LOG( WARNING ) << Name() << " : " << "Call response handler in ReleaseResponseTable NO " << i;
        handlers[i]( NULL, connection_ );
    }

    handlers.clear();
}

template <>
void RawConnection::InternalPushData< EncodeData >(const EncodeData &data ) {

    if ( data.first == NULL ) {
        LOG( WARNING ) << "Push NULL data!";
        return;
    }

    incoming()->push( data.first );
    incoming()->push( data.second );
}

boost::tribool
ProtobufDecoder::Consume( char input ) {
    switch ( mState ) {
        case End:
        case Start:
        {
            if ( !isdigit( input ) ) {
                LOG( WARNING ) << "Start but is not digit";
                return false;
            }
            mState = Length;
            length_store_.clear();
            length_store_.push_back( input );
            return boost::indeterminate;
        }
        case Length:
            if ( input == ':' ) {
                mState = Content;
                length_ = boost::lexical_cast<int>( length_store_ );
                content_.reserve( length_ );
                VLOG( 2 ) << "Length: " << length_store_ << " length size: " << length_store_.size() << " Content size: " << length_;
                return boost::indeterminate;

            } else if ( !isdigit( input ) ) {
                LOG( WARNING ) << "Length is not digit";
                return false;

            } else {
                length_store_.push_back( input );
                return boost::indeterminate;
            }
        case Content:

            if ( content_.full() ) {
                return false;
            }

            content_.push_back( input );

            if ( content_.full() ) {

                if ( !meta_.ParseFromArray( content_.content(), content_.capacity() ) ) {
                    LOG( WARNING ) << "Parse content error";
                    return false;
                }

                if ( meta_.type() == ProtobufLineFormat::MetaData::REQUEST &&
                        !meta_.has_response_identify() ) {
                    LOG( WARNING ) << "request meta data should have response identify field";
                    return false;
                }

                if ( meta_.content().empty() ) {
                    LOG( WARNING ) << "Meta without content: ";
                    return false;
                }

                mState = End;
                return true;
            }
            
            return boost::indeterminate;
        default:
            LOG( WARNING ) << "Unknown status of ProtobufDecoder";
            return false;
    }
}

bool
RawProtobufConnection::Handle( const ProtobufDecoder *decoder ) {
    CHECK( decoder != NULL );
    bool ret = mServiceConnection->Handle( connection_, decoder );

    if ( ret ) {
        RawConnTrace << "Client->Servive ";
        return true;
    }
    
    const ProtobufLineFormat::MetaData &meta = decoder->meta();
    HandlerTable::value_type::second_type handler;
    {
        boost::mutex::scoped_lock locker( mResponseHandlerTableMutex );
        HandlerTable::iterator it = mResponseHandlerTable.find( meta.identify() );

        if ( it == mResponseHandlerTable.end() ) {
            VLOG( 2 ) << Name() << " : " << "Unknown request";
            return false;
        }

        handler = it->second;
        mResponseHandlerTable.erase( it );
        RawConnTrace << "Remove: " << meta.identify() << " table size: " << mResponseHandlerTable.size();
    }

    handler( decoder, connection_ );
    return true;
}

RawProtobufConnection::RawProtobufConnection(
        const string    &name,
        ConnectionPtr   connection,
        ProtobufConnection *service_connection ) :
            RawConnectionImpl< ProtobufDecoder >( name, connection ),
            mServiceConnection( service_connection )
{ }

static void
CallMethodCallback(
        const ProtobufDecoder *decoder,
        ConnectionPtr connection,
        google::protobuf::RpcController *controller,
        google::protobuf::Message *response,
        google::protobuf::Closure *done ) {
    ScopedClosure run( done );
    RpcController *rpc_controller = dynamic_cast<RpcController*> ( controller );

    if ( decoder == NULL ) {
        VLOG( 2 ) << "NULL Decoder, may call from destructor";

        if ( rpc_controller ) {
            rpc_controller->SetFailed( "Abort" );
            rpc_controller->Notify();
        }
        return;
    }

    const ProtobufLineFormat::MetaData &meta = decoder->meta();

    VLOG( 2 ) << connection->Name() << " : " << "Handle response message "
            << response->GetDescriptor()->full_name()
            << " identify: " << meta.identify();

    if ( !response->ParseFromArray( meta.content().c_str(), meta.content().size() ) ) {
        LOG( WARNING ) << connection->Name() << " : " << "Fail to parse the response :";
        
        if ( rpc_controller ) {
            rpc_controller->SetFailed( "Fail to parse the response:" );
            rpc_controller->Notify();
            return;
        }
    }
    
    if ( rpc_controller ) rpc_controller->Notify();
}

void
RawProtobufConnection::CallMethod(
        RawConnection::StatusPtr status,
        const google::protobuf::MethodDescriptor *method,
        google::protobuf::RpcController *controller,
        const google::protobuf::Message *request,
        google::protobuf::Message *response,
        google::protobuf::Closure *done ) {

    VLOG( 2 ) << Name() << " : " << "CallMethod";

    uint64 request_identify = hash8( method->full_name() );
    uint64 response_identify = hash8( response->GetDescriptor()->full_name() );
    ProtobufLineFormat::MetaData meta;
    RpcController *rpc_controller = dynamic_cast<RpcController*> ( controller );
    bool error = false;
    string reason;
    EncodeData data;
    {
        boost::mutex::scoped_lock locker( mResponseHandlerTableMutex );
        HandlerTable::const_iterator it = mResponseHandlerTable.find( response_identify );

        while ( it != mResponseHandlerTable.end() ) {
            static int seq = 1;
            ++seq;
            response_identify += seq;
            it = mResponseHandlerTable.find( response_identify );
        }

        meta.set_identify( request_identify );
        meta.set_type( ProtobufLineFormat::MetaData::REQUEST );
        meta.set_response_identify( response_identify );
        mResponseHandlerTable.insert( make_pair(
                response_identify,
                boost::bind( CallMethodCallback, _1, _2, controller, response, done ) ) );

        error = !request->AppendToString( meta.mutable_content() );

        if ( error ) {
            LOG( WARNING ) << Name() << " : "
                    << "Fail to serialze request form method: "
                    << method->full_name();
            reason = "AppendTostringError";
            goto failed;
        }

        VLOG( 2 ) << Name() << " Insert: "
                << response_identify << " to response handler table, size: "
                << mResponseHandlerTable.size();
    }

    do {
        data = EncodeMessage( &meta );
        error = !PushData( data );

        if ( error ) {
            LOG( WARNING ) << Name() << " : " << "PushData error, connection may closed";
            reason = "PushDataError";
            delete data.first;
            delete data.second;
            break;
        }

        RawConnTrace << " PushData, " << " incoming: " << incoming()->size();
        error = !ScheduleWrite( status );
        
        if ( error ) {
            LOG( WARNING ) << Name() << " : "
                    << "ScheduleWrite error, connection may closed";
            reason = "ScheduleWriteError";
            delete data.first;
            delete data.second;
            break;
        }

        return;
    } while ( 0 );

failed:

    {
        boost::mutex::scoped_lock locker( mResponseHandlerTableMutex );
        mResponseHandlerTable.erase( response_identify );
    }

    if ( rpc_controller ) {
        rpc_controller->SetFailed( reason );
        rpc_controller->Notify();
    }

    if ( done ) {
        done->Run();
    }
}
#undef RawConnTrace
