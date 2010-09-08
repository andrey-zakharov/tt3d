/* 
 * File:   ClientConnection.hpp
 * Author: vaulter
 *
 * Created on 5 Сентябрь 2010 г., 17:32
 */

#ifndef CLIENTCONNECTION_HPP
#define	CLIENTCONNECTION_HPP

#include <boost/thread/shared_mutex.hpp>
#include "network/ProtobufConnection.hpp"
#include "network/IOServicePool.hpp"
#include "TimerMaster.hpp"

//class TimerMaster;
//class IOServicePool;
class ClientConnection : public ProtobufConnection {
 public:
  ClientConnection(const string &name, const string &server, const string &port);
  bool Connect();

  void SetIoServicePool(
      boost::shared_ptr< IOServicePool > io_service_pool) {
    io_service_pool_ = io_service_pool;
  }
  void SetTimerMaster(
      boost::shared_ptr< TimerMaster > timer_master) {
    timer_master_ = timer_master;
  }
  ~ClientConnection();
 private:
  static const int                      kClientThreadPoolSize = 1;
  boost::shared_ptr< IOServicePool >    io_service_pool_;
  boost::shared_ptr< TimerMaster >      timer_master_;
  string                                server_, port_;
};

#endif	/* CLIENTCONNECTION_HPP */

