#pragma once
#include <hydrogen/nio/iosocket.h>

namespace nio {
    class socket_acceptor : public tcp_socket {
    public:
        socket_acceptor();
        explicit socket_acceptor(const endpoint& ep);

        void bind(const endpoint& ep);
        void listen(int backlog = 5);
        void listen(const endpoint& ep, int backlog = 5);
        iosocket&& accept();

        endpoint getname() const { return _name; }

    private:
        /* The local endpoint that current acceptor binds to. */
        endpoint _name;
    };
}
