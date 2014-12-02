#pragma once
#include <hydrogen/nio/stream_socket.h>

namespace hy {
    class socket_acceptor : public tcp_socket {
    public:
        socket_acceptor();
        explicit socket_acceptor(const endpoint& ep, int backlog = 5);

        /* Supports move */
        socket_acceptor(socket_acceptor&& a);
        socket_acceptor& operator=(socket_acceptor&& a);

        void bind(const endpoint& ep);
        void listen(int backlog = 5);
        void listen(const endpoint& ep, int backlog = 5);

        stream_socket accept();

        /* Gets the local name of the acceptor. */
        endpoint getname() const {
            return _name;
        }

    private:
        /* The local endpoint that the acceptor binds to. */
        endpoint _name;
    };
}
