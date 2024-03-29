#include <boost/bind.hpp>
#include "TimerMaster.hpp"

using std::pair;
using std::make_pair;
void
TimerMaster::Start() {
    boost::mutex::scoped_lock lock( mutex_ );

    if ( !mbStop ) {
        return;
    }

    mbStop = false;
    timer_jiffies_ = 1;
    thread_.reset( new boost::thread( boost::bind( &TimerMaster::InternalRun, this ) ) );
}

void
TimerMaster::Stop() {
    {
        boost::mutex::scoped_lock lock( mutex_ );

        if ( mbStop ) {
            return;
        }

        mbStop = true;
    }

    thread_->join();
    VLOG( 2 ) << "Stopped TimerMaster";
}

void
TimerMaster::DestroyAllTimers() {

    for ( int i = 0; i < arraysize( vecs_ ); ++i ) {

        for ( int j = 0; j < kTVSize; ++j ) {
            TList *l = &vecs_[ i ][ j ];

            for ( TList::iterator it = l->begin(); it != l->end(); ) {
                TimerSlot *s = &*it;
                ++it;
                delete s;
            }
        }
    }
}

void
TimerMaster::InternalRun() {
    int jiffiers = 0;
    
    while ( !mbStop ) {
        sleep( 1 ); //bad
        Update( ++jiffiers );
    }
}

int
TimerMaster::CascadeTimers( TimerVec *v, int index ) {
    TList *l = &( *v )[ index ];
    
    for ( TList::iterator it = l->begin(); it != l->end(); ) {
        TimerSlot *s = &*it;
        ++it;
        s->unlink();
        InternalAddTimer( s, s->weak_timer, s->jiffies );
    }
    
    CHECK( l->empty() );
    return index;
}

#define INDEX( N ) (timer_jiffies_ >> (N * kTVBits)) & kTVMask

void
TimerMaster::Update( int jiffies ) {
    CHECK_GE( jiffies, timer_jiffies_ );
    TimerVec &v0 = vecs_[ 0 ];

    typedef vector< pair< TimerSlot*, boost::shared_ptr< Timer > > > TimersVector;

    while ( jiffies - timer_jiffies_ >= 0 ) {
        TimersVector timers;
        {
            boost::mutex::scoped_lock lock( mutex_ );
            const int index = timer_jiffies_ & kTVMask;

            if ( !index && !CascadeTimers( &vecs_[1], INDEX( 1 ) ) &&
                    !CascadeTimers( &vecs_[2], INDEX( 2 ) ) ) {
                CascadeTimers( &vecs_[3], INDEX( 3 ) );
            }
            
            TList *l = &v0[ index ];

            for ( TList::iterator it = l->begin(); it != l->end(); ) {
                TimerSlot *s = &*it;
                ++it;
                s->unlink();
                boost::weak_ptr<Timer> weak_timer = s->weak_timer;
                boost::shared_ptr<Timer> timer = weak_timer.lock();

                if ( weak_timer.expired() ) {
                    delete s;
                    continue;
                }

                timers.push_back( make_pair( s, timer ) );
            }
        }
        const int old_timer_jiffies = timer_jiffies_;
        ++timer_jiffies_;

        for ( TimersVector::iterator it = timers.begin(); it != timers.end(); ++it ) {
            it->second->Expired();
        }
        
        {
            for ( int i = 0; i < timers.size(); ++i ) {
                TimerSlot *s = timers[i].first;
                boost::shared_ptr< Timer > timer = timers[ i ].second;

                if ( timer->Period() ) {
                    boost::mutex::scoped_lock lock( mutex_ );
                    InternalAddTimer( s, timer, timer->Timeout() + old_timer_jiffies );
                } else {
                    delete s;
                }
            }
        }
    }
}

void
TimerMaster::InternalAddTimer(
        TimerSlot *slot,
        boost::weak_ptr<Timer> weak_timer,
        int jiffies ) {
    int i = 0;
    int idx = jiffies - timer_jiffies_;
    TList *v = NULL;
    int expires = jiffies;
    uint64 upper = 1;
    for ( int i = 0; i < arraysize( vecs_ ); ++i ) {
        upper <<= kTVBits;
        if ( idx < upper ) {
            int j = expires & kTVMask;
            v = &vecs_[i][j];
            break;
        }
        expires >>= kTVBits;
    }
    CHECK( v != NULL );
    slot->weak_timer = weak_timer;
    slot->jiffies = jiffies;
    v->push_back( *slot );
}

void
TimerMaster::Register( boost::weak_ptr< Timer > weak_timer ) {
    boost::mutex::scoped_lock locker( mutex_ );
    boost::shared_ptr< Timer > timer = weak_timer.lock();

    if ( weak_timer.expired() ) {
        return;
    }
    
    TimerSlot *slot = new TimerSlot;
    InternalAddTimer( slot, weak_timer, timer->Timeout() + timer_jiffies_ );
}
