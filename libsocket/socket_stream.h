#pragma once
#include "iosocket.h"

class socket_stream_exception : public std::exception {
public:
    socket_stream_exception() {}
    socket_stream_exception(const char* msg) : std::exception(msg) {}
};

class socket_stream {
public:
    socket_stream(iosocket&& sock) : _sock(std::move(sock)) {}

    bool open();

    void attach(iosocket&& sock) throw(socket_stream_exception){
        if (!_sock.is_bad()) {
            throw socket_stream_exception("can't attach socket to a open stream");
        }
        sock.move_to(_sock);
    }

private:
    iosocket _sock;
};
