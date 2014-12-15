#pragma once
#include <stdexcept>
#include <cassert>
#include <hydrogen/common/varadic.h>
#include <cerrno>

namespace hy {
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

    class exception {
    public:
        exception(const char* message = nullptr){
            if (message != nullptr){
                _message.assign(message);
            }
        }

        const std::string& what() const { return _message; }
    
    private:
        std::string _message;
    };

    class host_not_found : public exception {
    public:
        host_not_found(const char* host = nullptr)
            : exception("hy::host_not_found"){
            if (host){
                _host = host;
            }
        }

        const std::string& host() { return _host; }

    private:
        std::string _host;
    };

    class socket_exception : public exception {
    public:
        socket_exception(int error = socket_error::success)
            : exception("hy::socket_exception"), _error(error){}

        int error_code() const {
            return _error;
        }

    private:
        int _error;
    };
}
