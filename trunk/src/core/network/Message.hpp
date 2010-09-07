/* 
 * File:   Message.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 12:15
 */

#ifndef MESSAGE_HPP
#define	MESSAGE_HPP
#include <boost/shared_ptr.hpp>
#include "proto/meta.pb.h"

struct Message {
private:
    MetaData            mMeta;
public:
    std::string &  data() {};
};

/// class Reply -

class Reply : public Message {
};

/// class Request -

class Request : public Message {
};

/// class RequestHandler -

class MessageHandler {
    // Operations
public:
    void Handle( Request &request, Reply &reply );
};

typedef boost::shared_ptr< Message > MessagePtr;

#endif	/* MESSAGE_HPP */

