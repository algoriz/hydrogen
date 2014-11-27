#pragma once
#include <string>
#include <hydrogen/nio/inet_proto.h>
#include <hydrogen/nio/iosocket.h>

namespace nio{
    class socket_stream_exception : public std::exception {
    public:
        socket_stream_exception() {}
        socket_stream_exception(const char* msg) : std::exception(msg) {}
    };

    /* socket_stream encapsulates iosocket and provides std::iostream-like methods
     * to do synchronized(and blocked) IO.
     */
    class socket_stream {
    public:
        /* Constructs an empty socket_stream. */
        socket_stream();

        /* Constructs from an iosocket object.
         * The constructed socket_stream object will take the ownership of the
         * iosocket object.
         */
        socket_stream(iosocket&& sock);

        /* Attaches a new iosocket to current socket_stream.
         * Note that current socket_stream must be empty or closed before attaching
         * the new iosocket, or this method throws a socket_stream_exception.
         */
        void attach(iosocket&& sock);

        void open(const endpoint& ep);
        void open(const char* uname);

        void close(){ _sock.close(); }
        bool is_open() { return !_sock.is_bad(); }
        void read(char* buf, size_t bytes);
        void read_some(char* buf, size_t bytes);
        void write(const char* buf, size_t bytes);
        void write_some(const char* buf, size_t bytes);

        void getline(char* buf, size_t count, char delim = '\n');

        size_t tellg() const { return _sock.rdcount() - _buf.length(); }
        size_t tellp() const { return _sock.wrcount(); }
        bool can_read() const { return _sock.can_read(); }
        bool can_write() const { return _sock.can_write(); }

    private:
        size_t local_read(char*& buf, size_t& bytes);

    private:
        /* Underlying socket */
        iosocket _sock;

        /* Buffer for read parsing */
        std::string _buf;
    };
}