#pragma once
#include <string>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

namespace nio{
    class endpoint {
    public:
        endpoint();
        endpoint(const char* host, int port);

        /* Test whether the endpoint is invalid */
        bool bad() const {
            return _addr.sin_port == 0;
        }

        sockaddr_in getaddr() const {
            return _addr;
        }

        /* Converts to uniform address */
        std::string to_uname() const;

        /* Parses a uniform address.
        * format = tcp://<host>:<port>
        */
        static endpoint from_uname(const char* uname);

        /* endpoint bind to 127.0.0.1 */
        static endpoint localhost(int port){
            return endpoint("127.0.0.1", port);
        }

        /* endpoint bind to 0.0.0.0 */
        static endpoint any(int port){
            return endpoint("0.0.0.0", port);
        }

    private:
        sockaddr_in _addr;
    };
}
