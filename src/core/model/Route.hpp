/* 
 * File:   Route.hpp
 * Author: vaulter
 *
 * Created on 28 Сентябрь 2010 г., 22:30
 */

#ifndef ROUTE_HPP
#define	ROUTE_HPP
#include "Containers.hpp"
#include "Branch.hpp"

/**
 * Route, by ground and water
 */
class Route : public Branch {
public:
    Route( );
    Route( const Route& orig );
    virtual ~Route( );
private:
    list< BranchPtr >      branches;
    

};

#endif	/* ROUTE_HPP */

