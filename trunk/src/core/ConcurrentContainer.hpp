/* 
 * File:   MTContainer.hpp
 * Author: vaulter
 *
 * Created on 12 Сентябрь 2010 г., 1:56
 */

#ifndef MTCONTAINER_HPP
#define	MTCONTAINER_HPP
#include <boost/thread/mutex.hpp>

namespace base {

    template < typename I >
    struct EmptyUniquePredicate {
        bool
        operator()( const I &item ) {
            return false;
        }
    };


    template < typename Container, typename UniqPredicate = EmptyUniquePredicate< typename Container::value_type > >
    struct ConcurrentContainer {

        typedef     typename Container::value_type   value_type;
	typedef     typename Container::size_type    size_type;
        
        ConcurrentContainer( size_type startFrom = 1 ) {
            mpNextItemID = new size_type( startFrom );
        };
        
        /**
         * Will add client of not yet in list
         * @param client
         * @returns <b>0</b> on error, number of added
         */
        //virtual
        size_type
        Add( const value_type &item );


        /**
         * Checks index and return item with this
         */
        //virtual
        value_type
        Get( const size_type index ) const;

        inline
        size_type
        Size() const { return mItems.size(); }  //thread safe?

    protected:
        Container        mItems;

    private:
        boost::mutex    mItemsMutex;
        size_type       *mpNextItemID; ///<next client will get this hash

    };


}
#endif	/* MTCONTAINER_HPP */

