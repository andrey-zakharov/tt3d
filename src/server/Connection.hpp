/* 
 * File:   UserClient.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 1:11
 */

#ifndef USERCLIENT_HPP
#define	USERCLIENT_HPP

#include <boost/enable_shared_from_this.hpp>
#include "Network.hpp" // for user socket

  // bean
  /// class UserClient -
  class Connection : /* IF WITH_BOOST */public boost::enable_shared_from_this< Connection >
  {
    // Attributes
  public:
      
    typedef boost::shared_ptr< Connection > mPointer; //shared_ptr has to be only in 'define',' typedef'

    static mPointer
    create( IOService &io ) {
        return mPointer( new Connection( io ) ); //magic, magic :)
    }

    //ThreeDCoordinate camera;

    // Operations
  public:
    void Stop(); //disconnect

    TcpSocket &Socket() {
        return mSocket;
    }

    void Start() {
        /////// Here we go...
    }
  private:

      Connection( IOService & io ) : mSocket( io ) {};

      TcpSocket mSocket;

  };

  typedef boost::shared_ptr< Connection > ConnectionPtr; //to global

#endif	/* USERCLIENT_HPP */

