#include <cstdlib>
#include <hydrogen/nio/socket_stream.h>

using namespace nio;

socket_stream::socket_stream(){}

socket_stream::socket_stream(stream_socket&& sock) : _socket(std::move(sock)){}

socket_stream::socket_stream(const endpoint& ep){
    open(ep);
}

socket_stream& socket_stream::operator=(socket_stream&& s){
    if (this != &s){
        assert(_socket.bad());
        swap(s);
    }
    return *this;
}

void socket_stream::read(char* buf, size_t bytes) {
    local_read(buf, bytes);
    if (bytes) {
        _socket.read(buf, bytes);
    }
}

size_t socket_stream::read_some(char* buf, size_t bytes) {
    size_t rd = local_read(buf, bytes);
    if (!rd && bytes) {
        rd += _socket.read_some(buf, bytes);
    }
    return rd;
}

void socket_stream::write(const char* buf, size_t bytes) {
    _socket.write(buf, bytes);
}

size_t socket_stream::write_some(const char* buf, size_t bytes) {
    return _socket.write_some(buf, bytes);
}

size_t socket_stream::local_read(char*& buf, size_t& count){
    size_t rd = 0;
    if (!_buf.empty()){
        size_t buflen = _buf.length();
        rd = count < buflen ? count : buflen;
        _buf.copy(buf, rd);
        _buf.erase(0, rd);
        buf += rd;
        count -= rd;
    }
    return rd;
}

void socket_stream::open(const endpoint& ep){
    assert(_socket.bad());

    tcp_socket s = tcp_socket::new_socket();
    auto addr = ep.getaddr();
    if (::connect(s.native_handle(), (sockaddr*)&addr, sizeof(addr))){
        throw socket_exception(socket_error::last());
    }

    _socket.swap(stream_socket(
        std::move(s), stream_socket::readable | stream_socket::writable));
}

void socket_stream::open(const char* uname){
    open(endpoint::from_uname(uname));
}

void socket_stream::getline(char* dst, size_t count, char delim){
    bool delimed = false;
    if (!_buf.empty()){
        const char* src = _buf.c_str();
        size_t len = _buf.length();
        size_t max_bytes = count < len ? count : len;
        size_t bytes = 0;
        while (bytes < max_bytes && !(delimed = (src[bytes] == delim))){
            ++bytes;
        }
        memcpy(dst, src, bytes);
        _buf.erase(0, bytes + delimed);
        if (delimed || bytes == count){
            /* The getline request has been fulfilled by the buffer data */
            return ;
        }

        dst += bytes;
        count -= bytes;
    }

    /* All bytes in the buffer have been copied to dst, then we should read 
     * data from the socket.
     */
    size_t rd = _socket.read_some(dst, count);

    char* p = dst;
    char* pend = dst + rd;
    for (; !delimed && p < pend; ++p){
        delimed = (*p == delim);
    }

    if (delimed){
        *(p - 1) = '\0';
        
        /* Copy bytes after delimiter back to buffer */
        _buf.assign(p, pend - p);
    }
}
