#pragma once
#include <stdexcept>
#include <hydrogen/varadic.h>

namespace nio {
    struct exception : public std::exception {
        exception() : std::exception("Unknown nio::exception."){}
        exception(const char* msg) : std::exception(msg){}
        exception(const std::string& msg) : exception(msg.c_str()){}
    };

    class socket_exception : public exception {
    public:
        explicit socket_exception(int error = 0)
            : _error(error) {}

        socket_exception(const char* msg, int error = 0)
            : exception(msg), _error(error){}

        socket_exception(const std::string& msg, int error = 0)
            : exception(msg.c_str()), _error(error) {}

        int error_code() const { return _error; }

    private:
        int _error;
    };
}
