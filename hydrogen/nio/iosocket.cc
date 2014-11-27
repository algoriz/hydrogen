#include <hydrogen/nio/iosocket.h>

using namespace nio;

iosocket::iosocket() : _rwmask(0), _rdcount(0), _wrcount(0){}

iosocket::iosocket(socket_type&& sock, int rw)
    : tcp_socket(std::move(sock)), _rwmask(rw), _rdcount(0), _wrcount(0){}

iosocket::iosocket(iosocket&& s)
    : _rwmask(s._rwmask), _rdcount(s._rdcount), _wrcount(s._wrcount){
    s._rwmask = 0;
}

void iosocket::read(char* buf, size_t len, int flag) {
    while (len) {
        int r = read_some(buf, len, flag);
        buf += r;
        len -= r;
    }
}

void iosocket::write(const char* buf, size_t len, int flag){
    while (len) {
        int w = write_some(buf, len, flag);
        buf += w;
        len -= w;
    }
}

int iosocket::read_some(char* buf, size_t len, int flag){
    if (!can_read()){
        throw socket_exception("socket is not readable.", 0);
    }
    int rd = ::recv(native_handle(), buf, len, flag);
    if (rd <= 0){
        /* socket no longer readable */
        _rwmask &= ~readable;
        if (rd < 0){
            throw socket_exception("read operation on socket failed.", rd);
        }
    }
    _rdcount += rd;
    return rd;
}

int iosocket::write_some(const char* buf, size_t len, int flag) {
    if (!can_write()){
        throw socket_exception("socket is not writable.", 0);
    }
    int wr = ::send(native_handle(), buf, len, flag);
    if (wr <= 0){
        /* socket no longer readable */
        _rwmask &= ~writable;
        if (wr < 0){
            throw socket_exception("write operation on socket failed.", wr);
        }
    }
    _wrcount += wr;
    return wr;
}
