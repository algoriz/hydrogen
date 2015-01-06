#include <cstdlib>
#include <hydrogen/nio/socket_stream.h>

using namespace hy;

socket_stream::socket_stream()
    : _buf(4096){}

socket_stream::socket_stream(stream_socket&& sock)
    : _buf(4096), _socket(std::move(sock)){}

socket_stream::socket_stream(const endpoint& ep)
    : _buf(4096){
    open(ep);
}

socket_stream::socket_stream(socket_stream&& s)
    : _buf(std::move(s._buf)), _socket(std::move(s._socket)){
}

socket_stream& socket_stream::operator=(socket_stream&& s){
    if (this != &s){
        close();
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
        rd = _buf.copy(buf, count);
        _buf.pop(rd);
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
        throw io_exception("socket connect error");
    }

    auto tmp = stream_socket(std::move(s),
        stream_socket::readable | stream_socket::writable);
    _socket = std::move(tmp);
}

void socket_stream::open(const char* uname){
    open(endpoint(uname));
}

void socket_stream::getline(char* dst, size_t count, char delim){
    bool delimed = false;
    while (!delimed && count){
        if (_buf.empty()){
            _buf.trim();
            _buf.push(_socket.read_some(_buf.tail(), _buf.free()));
        }

        char* src = _buf.front();
        char* end = src + (_buf.length() < count ? _buf.length() : count);
        while (!delimed && src != end){
            delimed = (*src == delim);
            *dst++ = *src++;
        }

        size_t bytes = src - _buf.front();
        _buf.pop(bytes);
        count -= bytes;
    }

    if (delimed){
        *(dst - 1) = 0;
    }
}

int socket_stream::getch(){
    if (_buf.empty()){
        _buf.trim();
        _buf.push(_socket.read_some(_buf.tail(), _buf.free()));
    }

    int ch = EOF;
    if (!_buf.empty()){
        ch = *_buf.front();
        _buf.pop(1);
    }
    return ch;
}
