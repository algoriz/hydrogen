#pragma once
#include <stdexcept>
#include <cstdarg>
#include <cassert>
#include <cerrno>

#include <hydrogen/common/stdext.h>

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

    /* Base exception */
    class exception {
    public:
        exception(const char* message = nullptr){
            if (message != nullptr){
                _message.assign(message);
            }
        }

        exception(const std::string& message): _message(message){}
        exception(std::string&& message) : _message(std::move(message)){}
        exception(exception&& ex): _message(std::move(ex._message)){}

        const std::string& what() const { return _message; }
    
    private:
        std::string _message;
    };

    /* IO related exception */
    class io_exception : public exception {
    public:
        io_exception(const char* message = nullptr) : exception(message){}
        io_exception(const std::string& message) : exception(message){}
        io_exception(std::string&& message) : exception(std::move(message)){}
        io_exception(io_exception&& ex) : exception(std::move(ex)){}
    };

}
