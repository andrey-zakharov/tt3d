// http://code.google.com/p/server1/
//
// You can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Author: xiliu.tang@gmail.com (Xiliu Tang)

#ifndef HASH_HPP_
#define HASH_HPP_
#include <ext/hash_map>
#include <boost/shared_ptr.hpp>
#include "Common.hpp"
extern "C" uint64 hash8( const uint8 *, uint64, uint64 );

inline uint64
hash8( const string &x ) {
    return hash8( reinterpret_cast < const uint8* > ( x.c_str() ),
            static_cast < uint64 > ( x.size() ), 0xbeef );
}

#if defined(_MSC_VER)
// MSVC's stl implementation doesn't have this hash struct.

template <typename T>
struct hash {
    size_t operator()( const T & t ) const;
};
#else
namespace __gnu_cxx {

    template<> struct hash<void*> {

        size_t operator()( void* x ) const {
            return reinterpret_cast < size_t > ( x );
        }
    };

    template<> struct hash< int64 > {

        size_t operator()( int64 x ) const {
            return static_cast < size_t > ( x );
        }
    };

    template<> struct hash< uint64 > {

        size_t operator()( uint64 x ) const {
            return static_cast < size_t > ( x );
        }
    };

    template<> struct hash<string> {

        size_t operator()( const string & x ) const {
            return static_cast < size_t > ( hash8( x ) );
        }
    };

    template<class T> struct hash<T*> {

        size_t operator()( T * x ) const {
            return reinterpret_cast < size_t > ( x );
        }
    };

    template<class T> struct hash< boost::shared_ptr< T > > {

        size_t operator()( const boost::shared_ptr<T> &x ) const {
            return reinterpret_cast < size_t > ( x.get() );
        }
    };
}
#endif

#endif  // HASH_HPP_
