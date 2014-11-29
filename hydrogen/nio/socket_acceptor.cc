#include <hydrogen/nio/socket_acceptor.h>

using namespace nio;


socket_acceptor::socket_acceptor(): tcp_socket(false){}

socket_acceptor::socket_acceptor(const endpoint& ep)
    : tcp_socket(true){
    bind(ep);
    listen();
}

void socket_acceptor::bind(const endpoint& ep) {
    auto addr = ep.getaddr();
    if (::bind(native_handle(), (sockaddr*)&addr, sizeof(addr))) {
        throw socket_exception(hy::strcat("socket bind failed: ", ep.to_uname()), last_error());
    }
    _name = ep;
}

void socket_acceptor::listen(const endpoint& ep, int backlog) {
    if (bad()){
        tcp_socket::swap(tcp_socket::new_socket());
    }
    bind(ep);
    listen(backlog);
}

void socket_acceptor::listen(int backlog) {
    if (_name.bad()) {
        throw socket_exception("Socket not bind before listen.");
    }

    if (::listen(native_handle(), backlog)) {
        throw socket_exception(hy::strcat("socket listen failed: ", _name.to_uname()), last_error());
    }
}

stream_socket&& socket_acceptor::accept() {
    sockaddr_in addr;
    int len = sizeof(addr);
    int fd = ::accept(native_handle(), reinterpret_cast<sockaddr*>(&addr), &len);
    tcp_socket s(fd);
    if (s.bad()) {
        throw socket_exception("socket accept failed.");
    }
    return std::move(stream_socket(std::move(s)));
}
