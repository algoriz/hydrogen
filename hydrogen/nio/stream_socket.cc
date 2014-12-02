#include <hydrogen/nio/stream_socket.h>

using namespace hy;

stream_socket::stream_socket()
    : _rwmask(0), _bytes_in(0), _bytes_out(0){}

stream_socket::stream_socket(tcp_socket&& sock, int rw)
    : tcp_socket(std::move(sock)), _rwmask(rw), _bytes_in(0), _bytes_out(0){}

stream_socket::stream_socket(stream_socket&& s)
    : stream_socket(std::move(s), s._rwmask) {
    s._rwmask = 0;
}

stream_socket& stream_socket::operator=(stream_socket&& s){
    if (this != &s){
        close();
        swap(s);
    }
    return *this;
}

void stream_socket::read(char* buf, size_t len, int flag) {
    while (len) {
        auto r = read_some(buf, len, flag);
        buf += r;
        len -= r;
    }
}

void stream_socket::write(const char* buf, size_t len, int flag){
    while (len) {
        auto w = write_some(buf, len, flag);
        buf += w;
        len -= w;
    }
}

size_t stream_socket::read_some(char* buf, size_t len, int flag){
    //if (!can_read()){
    //    throw socket_exception("socket is not readable.", 0);
    //}
    int rd = ::recv(native_handle(), buf, len, flag);
    if (rd <= 0){
        /* socket no longer readable */
        _rwmask &= ~readable;
        if (rd < 0){
            throw socket_exception(socket_error::last());
        }
    }
    _bytes_in += rd;
    return rd;
}

size_t stream_socket::write_some(const char* buf, size_t len, int flag) {
    //if (!can_write()){
    //    throw socket_exception("socket is not writable.", 0);
    //}
    int wr = ::send(native_handle(), buf, len, flag);
    if (wr <= 0){
        /* socket no longer readable */
        _rwmask &= ~writable;
        throw socket_exception(socket_error::last());
    }
    _bytes_out += wr;
    return wr;
}

void stream_socket::close(){
    tcp_socket::close();
    _rwmask = 0;
    _bytes_in = 0;
    _bytes_out = 0;
}
