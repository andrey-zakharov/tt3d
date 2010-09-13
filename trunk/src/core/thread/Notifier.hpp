#ifndef NOTIFIER_HPP_
#define NOTIFIER_HPP_

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

using boost::posix_time::milliseconds;

class Notifier : public boost::enable_shared_from_this< Notifier >, public boost::noncopyable {
public:

    Notifier( const string name, int initial_count = 1 ) : mName( name ), count_( initial_count ), notified_( false ) { }

    const boost::function0< void >
    notify_handler( ) {
        return boost::bind( &Notifier::Notify, shared_from_this( ) );
    }

    void
    Notify( ) {
        Dec( 1 );
    }

    void
    Inc( int cnt ) {
        Dec( -1 * cnt );
    }

    void
    Dec( int cnt ) {
        boost::mutex::scoped_lock locker( mutex_ );
        if ( cnt > 0 && count_ >= cnt ) {
            count_ -= cnt;
        } else if ( cnt < 0 ) {
            count_ -= cnt;
        }
        VLOG( 2 ) << mName << " : " << "count: " << count_;
        if ( count_ == 0 ) {
            notify_.notify_all( );
            notified_ = true;
            VLOG( 2 ) << mName << " : " << "Notifed";
        }
        CHECK_GE( count_, 0 );
    }
    // Return true when notified, otherwise return false.

    bool
    Wait( ) {
        return Wait( LONG_MAX );
    }

    bool
    Wait( long timeout_ms ) {
        boost::mutex::scoped_lock locker( mutex_ );
        
        while ( !notified_ ) {
            VLOG( 2 ) << mName << " : " << "Wait";
            bool ret = notify_.timed_wait(
                    locker, boost::get_system_time() + milliseconds( timeout_ms ) );
            VLOG( 2 ) << mName << " : " << "Wait return";
            return ret;
        }
        
        return true;
    }

    int
    getCount( ) const {
        return count_;
    }
private:
    int                 count_;
    boost::mutex        mutex_;
    boost::condition    notify_;
    string              mName;
    bool                notified_;
};
#endif // NOTIFIER_HPP_
