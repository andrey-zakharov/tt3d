/* 
 * File:   IOServicePool.hpp
 * Author: vaulter
 *
 * Created on 5 Сентябрь 2010 г., 17:37
 */

#ifndef IOSERVICEPOOL_HPP
#define	IOSERVICEPOOL_HPP

#include <vector>
#include <boost/thread.hpp>
#include "thread/ThreadPool.hpp"
#include "Timer.hpp"
#include "Network.hpp"

/// A pool of io_service objects.
using std::string;
using std::vector;
using boost::shared_ptr;

class IOServicePoolTest;

class IOServicePool : private boost::noncopyable {
public:
    /// Construct the io_service pool.
    explicit IOServicePool(
            const string &name,
            size_t num_io_services,
            size_t num_threads );

    ~IOServicePool( ) {
        if ( IsRunning( ) ) {
            Stop( );
        }
    }

    /// Run all io_service objects in the pool.
    void Start( );

    /// Stop all io_service objects in the pool.
    void Stop( );

    bool
    IsRunning( ) const {
        return !mWork.empty( );
    }

    /// Get an io_service to use.
    IOService &GetIOService( );
private:

    /// The pool of io_services.
    vector< shared_ptr< IOService > > mIOServices;

    /// The work that keeps the io_services running.
    vector< shared_ptr< IOService::work > > mWork;

    int mNumIOServices;
    int mNumThreads;
    int mNextIOService;

    ThreadPool      mThreadPool;
    boost::mutex    mMutex;
    string          mName;

    friend class IOServicePoolTest;
};


#endif	/* IOSERVICEPOOL_HPP */

