#pragma once
#include <hydrogen/nio/protocols.h>

namespace hy{
    /*
     * stream_socket represents a TCP socket for blocked/synchronized IO.
     */
    class stream_socket : public tcp_socket {
    public:
        /* read/write mask bits */
        static const unsigned int readable = 0x01;
        static const unsigned int writable = 0x01 << 1;

        stream_socket();
        stream_socket(tcp_socket&& s, int rw);
        stream_socket(stream_socket&& s);

        stream_socket& operator= (stream_socket&& s);

        /* Read some bytes from the socket, returns number of bytes actually read.
         * If the read operation failed, this method throws an io_exception.
         */
        size_t read_some(char* buf, size_t len, int flag = 0);

        /* Write some bytes into the socket, returns number of bytes actually written.
         * If the write operation failed, this method throws an io_exception.
         */
        size_t write_some(const char* buf, size_t len, int flag = 0);

        /* Read specified number of bytes from the socket.
         * If the connection is closed or an error occurs before the specified bytes
         * are read, this method throws an io_exception.
         */
        void read(char* buf, size_t len, int flag = 0);

        /* Write specified number of bytes into the socket.
         * If the connection is closed or an error occurs before the specified bytes
         * are written, this method throws an io_exception.
         */
        void write(const char* buf, size_t len, int flag = 0);

        void swap(stream_socket& sock) {
            tcp_socket::swap(sock);
            std::swap(_rwmask, sock._rwmask);
            std::swap(_bytes_in, sock._bytes_in);
            std::swap(_bytes_out, sock._bytes_out);
        }

        /* Close the connection. */
        void close();

        /* Test whether the socket is readable */
        bool can_read() const { return 0 != (_rwmask & readable); }

        /* Test whether the socket is writable */
        bool can_write() const { return 0 != (_rwmask & writable); }

        /* Gets the total number of bytes read from the socket */
        size_t bytes_in() const { return _bytes_in; }

        /* Gets the total number of bytes written into the socket */
        size_t bytes_out() const { return _bytes_out; }

    private:
        /* read/write availability */
        unsigned int _rwmask;

        /* read/write counter */
        size_t _bytes_in;
        size_t _bytes_out;
    };
}

IMPLEMENT_STD_SWAP(hy::stream_socket)
