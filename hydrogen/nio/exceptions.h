#pragma once
#include <stdexcept>
#include <cassert>
#include <hydrogen/varadic.h>

namespace nio {
    /* Network IO specific error codes */
    class socket_error {
    public:
        static const int success = 0;
        static const int access_denied = EACCES;
        static const int address_in_use = EADDRINUSE;
        static const int bad_file_descriptor = EBADF;
        static const int not_socket = ENOTSOCK;
        static const int out_of_memory = ENOMEM;
        static const int operation_no_supported = EOPNOTSUPP;
        static const int connection_reset = 54;

        /* Get last error */
        static int last();
    };

    class host_not_found : public std::exception {
    public:
        host_not_found(const char* host = nullptr)
            : std::exception("nio::host_not_found"){
            if (host){
                _host = host;
            }
        }

        const std::string& host() { return _host; }

    private:
        std::string _host;
    };

    class socket_exception : public std::exception {
    public:
        socket_exception(int error = socket_error::success)
            : std::exception("nio::socket_exception"), _error(error){}

        int error_code() const {
            return _error;
        }

    private:
        int _error;
    };
}
