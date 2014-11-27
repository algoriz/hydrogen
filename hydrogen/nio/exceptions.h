#pragma once
#include <stdexcept>
#include <hydrogen/varadic.h>

namespace nio {
    struct exception : public std::exception {
        exception() : std::exception("Unknown nio::exception."){}
        exception(const char* msg) : std::exception(msg){}
        exception(const std::string& msg) : exception(msg.c_str()){}
    };

    class host_not_found : public exception {
    public:
        host_not_found(): exception("Host not found."){}
        host_not_found(const char* host) : exception(hy::makestr()){}
    };
}
