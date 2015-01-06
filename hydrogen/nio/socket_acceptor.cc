#include <hydrogen/nio/socket_acceptor.h>

using namespace hy;


socket_acceptor::socket_acceptor(){}

socket_acceptor::socket_acceptor(const endpoint& ep, int backlog) {
    listen(ep, backlog);
}

socket_acceptor::socket_acceptor(socket_acceptor&& a){
    swap(a);
}

socket_acceptor& socket_acceptor::operator=(socket_acceptor&& a){
    if (this != &a){
        assert(bad());
        tcp_socket::swap(a);
        _name = a._name;
    }
    return *this;
}

void socket_acceptor::bind(const endpoint& ep) {
    auto addr = ep.getaddr();
    if (::bind(native_handle(), (sockaddr*)&addr, sizeof(addr))) {
        std::string message = "bind error";
        message += ep.name();
    }
    _name = ep;
}

void socket_acceptor::listen(const endpoint& ep, int backlog) {
    if (bad()){
        auto tmp = tcp_socket::new_socket();
        tcp_socket::swap(tmp);
    }
    bind(ep);
    listen(backlog);
}

void socket_acceptor::listen(int backlog) {
    if (::listen(native_handle(), backlog)) {
        throw io_exception("socket listen error");
    }
}

stream_socket socket_acceptor::accept() {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = ::accept(native_handle(), reinterpret_cast<sockaddr*>(&addr), &len);
    if (fd == proto::badfd) {
        throw io_exception("socket accept error");
    }
    return stream_socket(
        tcp_socket(fd), stream_socket::readable | stream_socket::writable);
}
