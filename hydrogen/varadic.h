#pragma once
#include <string>

namespace hy {
    template<typename T, typename A1, typename ...Args>
    void append(T& t, A1 a1, Args... args){
        t += a1;
        append(t, args...);
    }

    template<typename T, typename A1>
    void append(T& t, A1 a1) {
        t += a1;
    }

    template<typename A1, typename ...Args>
    std::string&& strcat(A1 a1, Args... args){
        std::string tmp;
        append(tmp, a1, args...);
        return std::move(tmp);
    }
}
