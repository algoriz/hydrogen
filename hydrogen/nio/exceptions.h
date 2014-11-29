#pragma once
#include <stdexcept>
#include <thread>
#include <hydrogen/varadic.h>

namespace nio {
    /* Network IO specific error codes */
    class error_code {
    public:
        static const int success = 0;
        static const int access_denied = EACCES;
        static const int address_in_use = EADDRINUSE;
        static const int bad_file_descriptor = EBADF;
        static const int not_socket = ENOTSOCK;
        static const int out_of_memory = ENOMEM;
        static const int operation_no_supported = EOPNOTSUPP;

    };

    /* Get last error */
    int last_error();

    struct exception {
        exception()
            : _what("unknown nio::exception"), _error(error_code::success){}
        exception(int error)
            : _error(error){}
        exception(const char* msg, int error = error_code::success)
            : exception(std::string(msg), error){}
        exception(std::string&& msg, int error = error_code::success)
            : _error(error), _what(std::move(msg)){}

        int error() const { return _error; }
        const std::string& what() const { return _what; }

    private:
        int _error;
        std::string _what;
    };

    class socket_exception : public exception {
    public:
        socket_exception()
            : exception("unknown nio::socket_exception"){}
        socket_exception(const char* msg, int error = error_code::success)
            : exception(std::string(msg), error){}
        socket_exception(std::string&& msg, int error = error_code::success)
            : exception(std::move(msg), error){}
    };
}
