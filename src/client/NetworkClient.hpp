/* 
 * File:   NetworkClient.hpp
 * Author: vaulter
 *
 * Created on 4 Сентябрь 2010 г., 12:45
 */

#ifndef NETWORKCLIENT_HPP
#define	NETWORKCLIENT_HPP
#include <queue>
#include <string>
#include "network/Message.hpp"
#include "Network.hpp"

using std::string;
typedef std::queue< MessagePtr > MessageQueue; // TODO

namespace client {
    /// class NetworkClient -
    class NetworkClient {
        // Attributes
    public:
        MessageQueue writeMessages;
    private:
        IOService&      mIo;
        TcpSocket       mSocket;
        Message         mReadMessage;
        string          mServer,  mPort;
        // Operations
    public:
        NetworkClient( IOService& io, TcpResolver::iterator endpoint_iterator );
        void Close();

        /// Request to write
        /// @param message		(???)
        void Write( MessagePtr msg );


    private:
        void OnRead();
        void OnConnect( const boost::system::error_code& error, TcpResolver::iterator endpoint_iterator );
        void OnReadHeader();
        void OnReadBody();
        void DoWrite() ;// intermediant for io service post callback
        /// Async wrting to asio socket
        void OnWrite();
    protected:
        static const int    kBufferSize = 8192;

        typedef boost::array<char, kBufferSize> BufferType;
        
        BufferType          mBuffer_;
    };
}

#endif	/* NETWORKCLIENT_HPP */

