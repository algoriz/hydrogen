#pragma once
#include <hydrogen/nio/inet_proto.h>

namespace nio{
    /*
     * iosocket represents a TCP socket object for blocked/synchronized IO.
     * Value semantics is disabled, thus no copy or assignment is allowed.
     */
    class iosocket : public tcp_socket {
    public:
        /* read/write mask bits */
        static const int readable = 0x01;
        static const int writable = 0x01 << 1;

        iosocket();
        iosocket(tcp_socket&& sock, int rw = readable | writable);
        iosocket(iosocket&& sock);

        /* Read some bytes from the socket, returns number of bytes actually read.
         * If the read operation failed, this method throws a socket_exception.
         */
        int read_some(char* buf, size_t len, int flag = 0);

        /* Write some bytes into the socket, returns number of bytes actually written.
         * If the write operation failed, this method throws a socket_exception.
         */
        int write_some(const char* buf, size_t len, int flag = 0);

        /* Read specified bytes from the socket.
         * If the connection is closed or an error occurs before the specified bytes
         * are read, this method throws a socket_exception.
         */
        void read(char* buf, size_t len, int flag = 0);

        /* Write specified bytes into the socket.
         * If the connection is closed or an error occurs before the specified bytes
         * are written, this method throws a socket_exception.
         */
        void write(const char* buf, size_t len, int flag = 0);

        void swap(iosocket& sock) {
            tcp_socket::swap(sock);
            std::swap(_rwmask, sock._rwmask);
            std::swap(_rdcount, sock._rdcount);
            std::swap(_wrcount, sock._wrcount);
        }

        /* Close the connection. */
        void close();

        /* Test whether the socket is readable */
        bool can_read() const { return 0 != (_rwmask & readable); }

        /* Test whether the socket is writable */
        bool can_write() const { return 0 != (_rwmask & writable); }

        /* Gets the total number of bytes read from the socket */
        size_t rdcount() const { return _rdcount; }

        /* Gets the total number of bytes written into the socket */
        size_t wrcount() const { return _wrcount; }

    private:
        /* read/write availability */
        int _rwmask;

        /* read/write counter */
        size_t _rdcount;
        size_t _wrcount;
    };
}

IMPLEMENT_STD_SWAP(nio::iosocket)
