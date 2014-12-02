#pragma once
#include <string>
#include <hydrogen/nio/protocols.h>
#include <hydrogen/nio/stream_socket.h>
#include <hydrogen/common/queue_buffer.h>

namespace nio{
    /* socket_stream encapsulates stream_socket and provides std::iostream-like
     * methods to do synchronized(and blocked) IO.
     */
    class socket_stream {
    public:
        /* Constructs an empty socket_stream. */
        socket_stream();

        /* Opens a new socket_stream */
        socket_stream(const endpoint& ep);

        /* Constructs a socket_stream from an stream_socket object.
         * The constructed socket_stream object will take the ownership of the
         * stream_socket object.
         */
        socket_stream(stream_socket&& sock);
        
        socket_stream(socket_stream&& s);
        socket_stream& operator= (socket_stream&& s);

        /* Opens a new socket_stream. */
        void open(const endpoint& ep);
        void open(const char* uname);

        /* Closes the socket_stream. */
        void close(){
            _socket.close();
            _buf.release();
        }

        void swap(socket_stream& another){
            _socket.swap(another._socket);
            _buf.swap(another._buf);
        }

        bool is_open() { return !_socket.bad(); }
        void read(char* buf, size_t bytes);
        size_t read_some(char* buf, size_t bytes);
        void write(const char* buf, size_t bytes);
        size_t write_some(const char* buf, size_t bytes);

        void getline(char* buf, size_t count, char delim = '\n');
        int  getch();

        size_t tellg() const { return _socket.bytes_in() - _buf.length(); }
        size_t tellp() const { return _socket.bytes_out(); }
        bool can_read() const { return _socket.can_read(); }
        bool can_write() const { return _socket.can_write(); }

    private:
        size_t local_read(char*& buf, size_t& bytes);

    private:
        /* The underlying socket */
        stream_socket _socket;

        /* Buffer for socket reading */
        hy::queue_buffer<char> _buf;
    };
}

IMPLEMENT_STD_SWAP(nio::socket_stream)
