#pragma once
#include <hydrogen/nio/inet_proto.h>

namespace nio{
    class socket_exception : public std::exception {
    public:
        explicit socket_exception(int error = 0)
            : _error(error){}

        socket_exception(const char* msg, int error = 0)
            : _error(error), std::exception(msg) {}

        int error_code() const { return _error; }
    private:
        int _error;
    };

    /*
     * iosocket represents a TCP socket object for blocked/synchronized IO.
     * Value semantics is disabled, thus no copy or assignment is allowed.
     */
    class iosocket {
    public:
        /* read/write mask bits */
        static const int readable = 0x01;
        static const int writable = 0x01 << 1;

        iosocket();
        explicit iosocket(int fd, int rw = readable | writable);

        /* Enable move */
        iosocket(iosocket&& sock);

        /* Disable copy and assignment */
        iosocket(const iosocket& sock) = delete;
        iosocket& operator= (const iosocket& sock) = delete;

        ~iosocket();

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

        /* Test whether the underlying file descriptor is bad.
         * This method doesn't validate the file descriptor.
         */
        bool is_bad() const { return _fd == -1; }

        void swap(iosocket& sock) { std::swap(_fd, sock._fd); }

        /* Close the connection. */
        void close();

        /* Test whether the socket is readable */
        bool can_read() const { return 0 != (_rwmask & readable); }

        /* Test whether the socket is writable */
        bool can_write() const { return 0 != (_rwmask & writable); }

        /* Gets underlying native file descriptor */
        int native_handle() { return _fd; }

        /* Gets the total number of bytes read from the socket */
        size_t rdcount() const { return _rdcount; }

        /* Gets the total number of bytes written into the socket */
        size_t wrcount() const { return _wrcount; }

    private:
        /* The underlying file descriptor/handler. */
        int _fd;

        /* read/write availability */
        int _rwmask;

        /* read/write counter */
        size_t _rdcount;
        size_t _wrcount;
    };
}
