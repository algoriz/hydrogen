#pragma once
#include <cstring>
#include <cstdarg>
#include <string>

/* Helps to implements specialized std::swap<> method
 */
#define IMPLEMENT_STD_SWAP(name)\
template<>\
inline void ::std::swap(name& left, name& right) {\
    left.swap(right);\
}

namespace hy {
    /* Equivalent to memset(dst, 0, count) */
    inline void* zero_memory(void* dst, size_t count){
        return memset(dst, 0, count);
    }

    /* Composes a string using the given format and argument list. */
    inline std::string& format_v(std::string& result, const char* fmtstr, va_list args){
        int count = vsnprintf("", 1, fmtstr, args);
        result.assign(count, '\0');
        vsnprintf(&(result[0]), count, fmtstr, args);
        return result;
    }

    /* Composes a string using the given format and args. */
    inline std::string& format(std::string& result, const char* fmtstr, ...){
        va_list args;
        va_start(args, fmtstr);
        return format_v(result, fmtstr, args);
    }

    inline std::string format_v(const char* fmtstr, va_list args){
        std::string result;
        return std::move(format_v(result, fmtstr, args));
    }

    inline std::string format(const char* fmtstr, ...){
        va_list args;
        va_start(args, fmtstr);
        return std::move(format_v(fmtstr, args));
    }
}
