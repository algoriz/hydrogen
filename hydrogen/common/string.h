#pragma once
#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

namespace hy {
    class string;
    typedef std::vector<string> strings;
    typedef std::pair<string, string> keyval;

    /* hy::string is a lightweight C-style string wrapper.
     * The contents held by hy::string is immutable.
     */
    class string {
    public:
        typedef const char& reference;
        typedef const char* pointer;

        /* "Not a Position" indicator */
        static const size_t npos = (size_t)-1;

    public:
        /* Wraps an empty string */
        string()
            :_str(reinterpret_cast<const char*>(&_zero)),
            _end(reinterpret_cast<const char*>(&_zero)){}

        /* Wraps string str */
        string(pointer str)
            : _str(str), _end(str + strlen(str)){}

        /* Wraps initial count characters of string str */
        string(pointer str, size_t count)
            : _str(str), _end(str + count){}

        /* Wraps characters [beg : end] */
        string(pointer beg, pointer end)
            : _str(beg), _end(end) { assert(beg <= end); }

        /* Length of the string. */
        size_t length() const { return _end - _str; }

        /* Test whether the string is empty. */
        bool empty() const { return _end == _str; }

        /* Make this string empty. */
        void clear() { _str = _end = reinterpret_cast<const char*>(&_zero); }

        /* Remove leading spaces. */
        string& ltrim() {
            while (_str < _end && isspace(*_str)) { ++_str; }
            return *this;
        }

        /* Remove trailing spaces. */
        string& rtrim() {
            while (_end > _str && isspace(_end[-1])) { --_end; }
            return *this;
        }

        /* Remove leading and trailing spaces. */
        string& trim() {
            return ltrim().rtrim();
        }

        /* Remove at most n characters at back. */
        string& pop_back(size_t n){
            if (_end - n > _str){
                _end -= n;
            }
            else {
                clear();
            }
            return *this;
        }

        /* Remove at most n characters at front. */
        string& pop_front(size_t n){
            if (_str + n < _end){
                _str += n;
            }
            else {
                clear();
            }
            return *this;
        }

        /* Converts to std::string */
        std::string std_string() const {
            return std::string(_str, length());
        }

        /* Get the string buffer.
         * Note that the string buffer MAY NOT be null terminated.
         */
        pointer buffer() const { return _str; }

        /* Forward iterators */
        pointer begin() const { return _str; }
        pointer end() const { return _end; }

        reference& front() const { return *_str; }
        reference& back() const { return *(_end - 1); }

        /* Get substring[start : stop]
         * Both start and stop can be negative. A negative value indicates that
         * the position is counted backward starting from the end of the string.
         */
        string substr(int start = 0, int stop = INT_MAX) const {
            auto a = _offset(start);
            auto b = _offset(stop);
            return a < b ? string(a, b) : string();
        }

        /* Find the position of the first occurrence of ch in the string.
         * If ch is not found in string, npos is returned.
         */
        size_t find(char ch) const {
            for (auto p = _str; p != _end; ++p){
                if (*p == ch){
                    return p - _str;
                }
            }
            return npos;
        }
        
        /* Find the position of the last occurrence of ch in the string.
         * If ch is not found in string, npos is returned.
         */
        size_t rfind(char ch) const {
            auto rend = _str - 1;
            for (auto p = _end - 1; p != rend; --p){
                if (*p == ch){
                    return p - _str;
                }
            }
            return npos;
        }

        /* Search string for the first occurrence of str.
         * If str is not found in string, npos is returned.
         */
        size_t search(const string& str) const {
            auto p = std::search(_str, _end, str._str, str._end);
            return p == _end ? npos : p - _str;
        }

        bool contains(char ch) const {
            return find(ch) != npos;
        }

        bool contains(const string& str) const {
            return search(str) != npos;
        }

        /* Compare this string to str. */
        int compare(const char* str) const {
            int diff = memcmp(_str, str, length());
            return diff ? diff : -str[length()];
        }

        /* Compare this string to str[0:count]. */
        int compare(const char* str, size_t count) const {
            if (count > length()){
                count = length();
            }
            int diff = memcmp(_str, str, count);
            return diff ? diff : (int)_str[count] - str[count];
        }

        /* Compare this string to str. */
        int compare(const string& str) const {
            return compare(str._str, str.length());
        }

        bool starts_with(const char* str) const {
            const char* p = str;
            const char* q = _str;
            while (*p && *p == *q && ++p && ++q);
            return 0 == *p;
        }

        bool starts_with(const string& str) const {
            size_t rlen = str.length();
            return length() >= rlen
                && 0 == memcmp(_str, str._str, rlen);
        }

        bool ends_with(const char* str) const {
            return ends_with(string(str));
        }

        bool ends_with(const string& str) const {
            size_t rlen = str.length();
            return length() >= rlen
                && 0 == memcmp(_end - rlen, str._str, rlen);
        }

        bool equals(const char* str) const {
            return equals(string(str));
        }

        bool equals(const string& str) const {
            return length() == str.length()
                && 0 == memcmp(_str, str._str, length());
        }

        /* Copy the whole string to `dst` and returns `dst`.
         * This method assumes that size of `dst` is sufficient to store the 
         * whole string including the terminating null character.
         */
        char* copy(char* dst) const {
            size_t mylen = length();
            memmove(dst, _str, mylen);
            dst[mylen] = 0;
            return dst;
        }

        /* Copy initial `count` characters to `dst` and returns `dst`.
         * If `count` is less than or equal to the length of the string, a null
         * character is not automatically appended to `dst`.
         * If `count` is greater than the length of the string, `dst` is padded
         * with null characters up to length `count`.
         */
        char* ncopy(char* dst, size_t count) const {
            size_t mylen = length();
            if (count > mylen){
                memmove(dst, _str, mylen);
                memset(dst + mylen, 0, count - mylen);
            }
            else {
                memmove(dst, _str, count);
            }
            return dst;
        }

        /* Splits the string into up to (`splits`+1) parts with `ch` as the separator.
         * The split result will be appended to `parts`.
         * Returns `parts`.
         */
        strings& split(strings& parts, char ch, size_t splits = -1) const {
            auto a = _str, b = a;
            while (b < _end){
                if (*b == ch){
                    parts.push_back(string(a, b));
                    a = b + 1;
                    if (--splits == 0){
                        b = _end;
                        break;
                    }
                }
                ++b;
            }

            if (a != b){
                parts.push_back(string(a, b));
            }
            return parts;
        }

        /* Splits the string into up to (`splits`+1) parts with `s` as the separator.
         * The split result will be appended to `parts`.
         * Returns `parts`.
         */
        strings& split(strings& parts, const string& s, size_t splits = -1) const {
            _check_separator(s);

            auto a = _str, b = a;
            const size_t step = s.length();
            while (b < _end){
                b = std::search(a, _end, s._str, s._end);
                if (b != _end){
                    parts.push_back(string(a, b));
                    b += step;
                    a = b;
                    if (--splits == 0){
                        b = _end;
                    }
                }
            }

            if (a != b){
                parts.push_back(string(a, b));
            }
            return parts;
        }

        /* Splits the string into up to (`splits`+1) parts with `ch` as the separator. */
        strings split(char ch, size_t splits = -1) const {
            std::vector<string> parts;
            return std::move(split(parts, ch, splits));
        }
        
        /* Splits the string into up to (`splits`+1) parts with `s` as the separator. */
        strings split(const string& s, size_t splits = -1) const {
            std::vector<string> parts;
            return std::move(split(parts, s, splits));
        }

        /* Splits the string into N (requires N > 0) parts with `ch` as the separator.
         * The split result will be assigned to `parts`.
         * Returns `parts`.
         */
        template<size_t N>
        std::array<string, N>& split_n(char ch, std::array<string, N>& parts) const {
            auto a = _str, b = a;
            size_t n = 0;
            for (; b < _end; ++b){
                if (*b == ch){
                    if (N == ++n){
                        b = _end;
                    }
                    parts[n-1]._assign(a, b);
                    a = b + 1;
                }
            }

            if (a != b){
                parts[n]._assign(a, b);
                ++n;
            }
            for (; n < N; ++n){
                parts[n].clear();
            }
            return parts;
        }
        
        /* Splits the string into N (requires N > 0) parts with `s` as the separator.
         * The split result will be assigned to `parts`.
         * Returns `parts`.
         */
        template<size_t N>
        std::array<string, N>& split_n(const string& s, std::array<string, N>& parts) const {
            _check_separator(s);

            auto a = _str, b = a;
            size_t n = 0;
            const size_t step = s.length();
            while (b < _end){
                b = std::search(a, _end, s._str, s._end);
                if (b != _end){
                    if (N == ++n){
                        b = _end;
                    }
                    parts[n-1]._assign(a, b);
                    b += step;
                    a = b;
                }
            }

            if (a != b){
                parts[n]._assign(a, b);
                ++n;
            }
            for (; n < N; ++n){
                parts[n].clear();
            }
            return parts;
        }

        /* Splits the string into N (requires N > 0) parts with `ch` as the separator. */
        template<size_t N>
        std::array<string, N> split_n(char ch) const {
            std::array<string, N> parts;
            return split_n(ch, parts);
        }

        /* Splits the string into N (requires N > 0) parts with `s` as the separator. */
        template<size_t N>
        std::array<string, N> split_n(const string& s) const {
            std::array<string, N> parts;
            return split_n(s, parts);
        }
        
        /* Splits the string into key/val pair with `ch` as the separator.
         * Returns `kv`.
         */
        keyval& split_kv(char ch, keyval& kv) const {
            auto p = _str;
            while (p < _end && *p != ch){
                ++p;
            }
            kv.first._assign_checked(_str, p);
            kv.second._assign_checked(p + 1, _end);
            return kv;
        }

        /* Splits the string into key/val pair with string `s` as the separator.
         * Returns `kv`.
         */
        keyval& split_kv(const string& s, keyval& kv) const {
            _check_separator(s);

            auto p = std::search(_str, _end, s._str, s._end);
            kv.first._assign_checked(_str, p);
            kv.second._assign_checked(p + s.length(), _end);
            return kv;
        }

        /* Splits the string into key/val pair with `ch` as the separator. */
        keyval split_kv(char ch) const {
            std::pair<string, string> kv;
            return split_kv(ch, kv);
        }

        /* Splits the string into key/val pair with `s` as the separator. */
        keyval split_kv(const string& s) const {
            std::pair<string, string> kv;
            return split_kv(s, kv);
        }

        /* Gets the integer value represented by the string. */
        int to_int() const {
            if (*_end < '0' || *_end > '9'){
                return atoi(_str);
            }

            char d[36];
            d[34] = d[35] = '\0';
            size_t len = length() > 34 ? 34 : length();
            return atoi(ncopy(d, len));
        }

        /* Gets the double value represented by the string. */
        double to_double() const {
            if (*_end < '0' || *_end > '9'){
                return atof(_str);
            }

            char d[36];
            d[34] = d[35] = '\0';
            size_t len = length() > 34 ? 34 : length();
            return atof(ncopy(d, len));
        }

        /* Gets the long long value represented by the string. */
        long long to_longlong() const {
            if (*_end < '0' || *_end > '9'){
                return atoll(_str);
            }

            char d[36];
            d[34] = d[35] = '\0';
            size_t len = length() > 34 ? 34 : length();
            return atoll(ncopy(d, len));
        }

        /* Random access without checking. */
        const char& operator[](size_t i) const { return _str[i]; }

        /* Substring operator. Returns substring[start : stop] */
        string operator()(int start = 0, int stop = INT_MAX) const {
            return substr(start, stop);
        }

    private:        
        pointer _offset(int n) const {
            if (n >= 0){
                return _str + n < _end ? _str + n : _end;
            }
            return _end + n > _str ? _end + n : _str;
        }

        void _assign(pointer str, pointer end){
            _str = str;
            _end = end;
        }

        void _assign_checked(pointer str, pointer end){
            if (end > str){
                _str = str;
                _end = end;
            }
            else {
                clear();
            }
        }

        static void _check_separator(const string& s) {
            if (s.empty()){
                throw std::invalid_argument("separator string can't be empty");
            }
        }

        static const int _zero = 0;
        pointer _str;
        pointer _end;
    };

    inline bool operator== (const string& left, const string& right){
        return left.equals(right);
    }

    inline bool operator== (const string& left, const char* right){
        return left.equals(right);
    }

    inline bool operator== (const char* left, const string& right){
        return right.equals(left);
    }

    inline bool operator!= (const string& left, const string& right){
        return !left.equals(right);
    }

    inline bool operator!= (const string& left, const char* right){
        return !left.equals(right);
    }

    inline bool operator!= (const char* left, const string& right){
        return !right.equals(left);
    }

    /* Ordering */
    inline bool operator< (const string& left, const string& right){
        return left.compare(right) < 0;
    }

    inline bool operator< (const string& left, const char* right){
        return left.compare(right) < 0;
    }

    inline bool operator< (const char* left, const string& right){
        return right.compare(left) < 0;
    }

    inline std::ostream& operator<< (std::ostream& out, const string& str){
        out.write(str.buffer(), str.length());
        return out;
    }
}
