#include <algorithm>
#include "ConcurrentContainer.hpp"

namespace base {

    template< typename Container, typename UniqPredicate >
    typename ConcurrentContainer< Container, UniqPredicate >::size_type
    ConcurrentContainer< Container, UniqPredicate >::Add(
        const typename ConcurrentContainer< Container, UniqPredicate >::value_type &item
    ) {

        boost::mutex::scoped_lock locker( this->mItemsMutex );
        
        UniqPredicate uniq_predicate( item ); //<- generalization

        //such conditions loops over container several times. needs to be optimized
        if( ( find( mItems.begin(), mItems.end(), item ) == mItems.end() ) &&
            ( find_if( mItems.begin(), mItems.end(), uniq_predicate ) == mItems.end() )
        ) {
            mItems.push_back( item );
            return mItems.size();// will be 1 for first.. TODO - via GetNextId() :)
        }

        return 0;
    };


    template< typename Container, typename UniqPredicate >
    typename ConcurrentContainer< Container, UniqPredicate >::value_type
    ConcurrentContainer< Container, UniqPredicate >::Get( const typename ConcurrentContainer< Container, UniqPredicate >::size_type index  ) const {

        //boost::mutex::scoped_lock locker( this->mClientsListMutex );
        // here we need to choosr between locks or function const
        //will see to usage of this class

        if ( index && index <= Size() ) { //IsValidID() TODO

            return mItems.at( index - 1 );
        }

        // else to prevent disharmony, create empty
        return     ConcurrentContainer::value_type();
    }
}
