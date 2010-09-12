/* 
 * File:   Containers.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 0:26
 */

#ifndef CONTAINERS_HPP
#define	CONTAINERS_HPP
#include "network/Hash.hpp"
/**
 * code include this header for list, vector, ....
 * this file candidate for redefining by
 * #ifdef _WITH_OGRE
 * #ifdef _WITH_IRR
 *
 */

#include <list>
using               std::list;
#define List        std::list

#include <map>
using               std::map;
//#include <base/hash.hpp>

#include <ext/hash_map>
#include <ext/hash_set>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;


#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;
using boost::scoped_array;

using std::make_pair;

#endif	/* CONTAINERS_HPP */

