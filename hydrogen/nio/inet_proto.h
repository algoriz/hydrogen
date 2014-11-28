#pragma once
#include <hydrogen/nio/exceptions.h>
#include <hydrogen/stdhelp.h>

#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")
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
            return endpoint("localhost", port);
        }

        /* endpoint bind to 0.0.0.0 */
        static endpoint any(int port){
            return endpoint("0.0.0.0", port);
        }

    private:
        sockaddr_in _addr;
    };

    struct proto_traits_tag {
        /*
         *  static const int badfd;
         *  static int create();
         *  static void close(int fd);
         */
    };

    /* TCP protocol traits */
    struct proto_tcp : public proto_traits_tag {
        static const int badfd = -1;

        static int create() {
            return ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }

        static void close(int fd) {
            if (fd != badfd) {
                ::shutdown(fd, 2);
                ::closesocket(fd);
            }
        }
    };

    template<typename proto_traits>
    class socket_base {
    public:
        typedef proto_traits proto;
        typedef socket_base<proto> socket_type;

        /* Disable value semantics. Copy or assignment is not allowed. */
        socket_base(const socket_base&) = delete;
        socket_base& operator=(const socket_base&) = delete;

        explicit socket_base(int fd) : _fd(fd) {}

        explicit socket_base(bool init = false)
            : _fd(init ? proto::create() : proto::badfd) {
            if (init && _fd == proto::badfd) {
                throw socket_exception("Failed to create a new socket.");
            }
        }

        socket_base(socket_base&& s) : _fd(s._fd) {
            s._fd = proto::badfd;
        }

        ~socket_base() { close(); }

        /* Test whether the underlying file descriptor is bad.
         * This method doesn't validate the file descriptor.
         */
        bool bad() const { return _fd == proto::badfd; }

        /* Close the socket. */
        void close() { proto::close(_fd); }

        void swap(socket_base& another) {
            std::swap(_fd, another._fd);
        }

        int native_handle() { return _fd; }

    private:
        int _fd;
    };

    typedef socket_base<proto_tcp> tcp_socket;
}

IMPLEMENT_STD_SWAP(nio::tcp_socket)
