/* 
 * File:   CargoType.hpp
 * Author: vaulter
 *
 * Created on 13 Октябрь 2010 г., 0:16
 */

#ifndef CARGOTYPE_HPP
#define	CARGOTYPE_HPP
#include <string>
namespace model {

    typedef int     CargoTypeID;

    struct CargoType {
    public:
        CargoType( );
        CargoType( const CargoType& orig );
        virtual ~CargoType( );

        CargoTypeID         id; ///<unique and constant
        std::string              name;
        //type;
    private:

    };
}

#endif	/* CARGOTYPE_HPP */

