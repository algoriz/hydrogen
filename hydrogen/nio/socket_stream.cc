#include <cstdlib>
#include <hydrogen/nio/socket_stream.h>

using namespace nio;

socket_stream::socket_stream(){}

socket_stream::socket_stream(iosocket&& sock) : _sock(std::move(sock)){}

void socket_stream::attach(iosocket&& sock){
    if (!_sock.is_bad()) {
        throw socket_stream_exception("can't attach socket to a open stream");
    }
    sock.swap(_sock);
}

void socket_stream::read(char* buf, size_t bytes){
    try{
        local_read(buf, bytes);
        if (bytes){
            _sock.read(buf, bytes);
        }
    }
    catch (socket_exception e){
        throw socket_stream_exception(e.what());
    }
}

void socket_stream::read_some(char* buf, size_t bytes){
    try{
        if (!local_read(buf, bytes) && bytes){
            _sock.read_some(buf, bytes);
        }
    }
    catch (socket_exception e){
        throw socket_stream_exception(e.what());
    }
}

void socket_stream::write(const char* buf, size_t bytes){
    try{
        _sock.write(buf, bytes);
    }
    catch (std::exception e){
        throw socket_stream_exception(e.what());
    }
}

void socket_stream::write_some(const char* buf, size_t bytes){
    try{
        _sock.write_some(buf, bytes);
    }
    catch (std::exception e){
        throw socket_stream_exception(e.what());
    }
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
    int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1){
        throw socket_stream_exception("failed to create socket.");
    }

    auto addr = ep.getaddr();
    if (::connect(fd, (sockaddr*)&addr, sizeof(addr))){
        throw socket_stream_exception("could not connect.");
    }

    _sock.swap(iosocket(fd));
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
    int rd = 0;
    try{
        rd = _sock.read_some(dst, count);
    }
    catch (socket_exception e){
        throw socket_stream_exception(e.what());
    }
    
    char* p = dst;
    char* pend = dst + rd;
    for (; !delimed && p < pend; ++p){
        delimed = (*p == delim);
    }

    if (delimed){
        *p = '\0';
        ++p;
        /* Copy bytes after delimiter back to buffer */
        _buf.assign(p, pend - p);
    }
}