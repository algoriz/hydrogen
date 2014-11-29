#pragma once
#include <string>
#include <hydrogen/nio/protocols.h>
#include <hydrogen/nio/stream_socket.h>

namespace nio{
    /* socket_stream encapsulates stream_socket and provides std::iostream-like
     * methods to do synchronized(and blocked) IO.
     */
    class socket_stream {
    public:
        /* Constructs an empty socket_stream. */
        socket_stream();

        /* Constructs from an stream_socket object.
         * The constructed socket_stream object will take the ownership of the
         * stream_socket object.
         */
        socket_stream(stream_socket&& sock);

        /* Attaches a new stream_socket to current socket_stream.
         * Note that current socket_stream must be empty or closed before attaching
         * the new stream_socket, or this method throws a socket_stream_exception.
         */
        void attach(stream_socket&& sock);

        void open(const endpoint& ep);
        void open(const char* uname);

        void close(){ _sock.close(); }
        bool is_open() { return !_sock.bad(); }
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
        /* The underlying socket */
        stream_socket _sock;

        /* Buffer for read parsing */
        std::string _buf;
    };
}
