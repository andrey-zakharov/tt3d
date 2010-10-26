/* 
 * File:   Common.hpp
 * Author: vaulter
 *
 * Created on 6 Сентябрь 2010 г., 3:57
 */

#ifndef COMMON_HPP
#define	COMMON_HPP
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#define foreach    BOOST_FOREACH

using std::string;
using std::vector;


template < class T, int N >
inline const int
arraysize( T( &x )[ N ] ) {
    return N;
}


//to Types.hpp
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
#ifdef COMPILER_MSVC
typedef __int64             int64;
typedef unsigned __int64    uint64;
#else
typedef long long           int64;
typedef unsigned long long  uint64;
#endif /* COMPILER_MSVC */

#endif	/* COMMON_HPP */

