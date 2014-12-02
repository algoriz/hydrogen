#pragma once

/* Helps to implements specialized std::swap<> method
 */
#define IMPLEMENT_STD_SWAP(name)\
template<>\
inline void ::std::swap(name& left, name& right) {\
    left.swap(right);\
}
