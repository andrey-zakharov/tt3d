// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#ifndef SHARED_CONST_BUFFERS_HPP_
#define SHARED_CONST_BUFFERS_HPP_

#include "Common.hpp"
#include <boost/asio.hpp>


class SharedConstBuffers {
private:

    struct Store {
        vector< const string * > data;

        ~Store( ) {
            for ( int i = 0; i < data.size( ); ++i ) {
                VLOG( 2 ) << "SharedConstBuffers Store delete: " << data[ i ];
                delete data[ i ];
            }
            
            data.clear( );
        }
    };
public:
    // Implement the ConstBufferSequence requirements.
    typedef boost::asio::const_buffer   value_type;
    typedef vector< boost::asio::const_buffer >::const_iterator const_iterator;

    const const_iterator
    begin( ) const {
        return mBuffer.begin( ) + mStart;
    }

    const const_iterator
    end( ) const {
        return mBuffer.end( );
    }

    SharedConstBuffers( ) : mStart( 0 ), mStore( new Store ) { }

    void
    push( const string *data ) {
        VLOG( 2 ) << "SharedConstBuffers push: " << data;
        mStore->data.push_back( data );
        mBuffer.push_back( boost::asio::const_buffer( data->c_str( ), data->size( ) ) );
    }

    void
    clear( ) {
        VLOG( 2 ) << "Clear SharedConstBuffers";
        mBuffer.clear( );
        mStore.reset( new Store );
        mStart = 0;
    }

    bool
    empty( ) const {
        return (mStart == mBuffer.size( ) );
    }

    int
    size( ) const {
        int s = 0;
        for ( int i = mStart; i < mBuffer.size( ); ++i ) {
            const int bsize = boost::asio::buffer_size( mBuffer[i] );
            s += bsize;
        }
        return s;
    }

    void
    consume( int size ) {
        for ( int i = mStart; i < mBuffer.size( ); ++i ) {
            const int bsize = boost::asio::buffer_size( mBuffer[i] );
            if ( size > bsize ) {
                size -= bsize;
            } else if ( size == bsize ) {
                mStart = i + 1;
                return;
            } else {
                mBuffer[ i ] = mBuffer[ i ] + size;
                mStart = i;
                return;
            }
        }
        mStart = mBuffer.size( );
    }
private:
    vector< boost::asio::const_buffer >       mBuffer;
    boost::shared_ptr< Store >                mStore;
    int                                       mStart;
};
#endif // SHARED_CONST_BUFFERS_HPP_
