#pragma once
#include <string>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <array>
#include <algorithm>

namespace hy {
    /* hy::string is a wrapper over C-style string(char array), it provides a
     * write restricted std::string-like interface.
     * 
     * The most notable difference between hy::string and std::string is that
     * hy::string doesn't has built-in memory management. Any string operation
     * that will induce or possibly induce memory growth is not supported.
     */
    class string {
    public:
        /* empty string */
        string() : _str(_zeros()), _end(_zeros()) {}

        /* Wraps the initial count characters of str as a string. */
        string(const char* str, size_t count = -1){ wrap(str, count); }

        /* Reset the wrapped buffer to the initial count characters of str. */
        string& wrap(const char* str, size_t count = -1){
            assert(str != nullptr);
            _str = str;
            if (count == -1){
                count = strlen(str);
            }
            _end = _str + count;
            return *this;
        }

        /* Make the string buffer null terminated. */
        string& make_cstr(){
            if (_end != _zeros()){
                *const_cast<char*>(_end) = 0;
            }
            return *this;
        }

        /* Make characters to lower case. */
        string& make_lower(){
            for (char* p = begin(); p != _end; ++p){
                *p = tolower(*p);
            }
            return *this;
        }

        /* Make characters to upper case. */
        string& make_upper(){
            for (char* p = begin(); p != _end; ++p){
                *p = toupper(*p);
            }
            return *this;
        }

        /* Remove leading spaces. */
        string& ltrim() {
            while (_str < _end && isspace(*_str)) {
                ++_str;
            }
            return *this;
        }

        /* Remove trailing spaces. */
        string& rtrim() {
            while (_end > _str && isspace(_end[-1])) {
                --_end;
            }
            return *this;
        }

        /* Remove leading and trailing spaces. */
        string& trim() {
            return ltrim().rtrim();
        }

        /* Remove n characters at back. */
        string& pop_back(size_t n){
            if (n > length()){
                clear();
            }
            else {
                _end -= n;
            }
            return *this;
        }

        /* Remove n characters at front. */
        string& pop_front(size_t n){
            if (n > length()){
                clear();
            }
            else {
                _str += n;
            }
            return *this;
        }

        /* Make this string empty. */
        void clear() { _str = _end = _zeros(); }

        /* Length of the string. */
        size_t length() const { return _end - _str; }

        /* Test whether the string is empty. */
        bool empty() const { return _end == _str; }

        /* Converts to std::string */
        std::string std_string() const {
            return std::string(_str, length());
        }

        /* Get the string buffer which MAY NOT be null terminated. */
        const char* buffer() const { return _str; }

        /* iterators */
        char* begin() const { return const_cast<char*>(_str); }
        char* end() const { return const_cast<char*>(_end); }
        const char* cbegin() const { return _str; }
        const char* cend() const { return _end; }

        /* Backward iterators */
        char* rbegin() const { return const_cast<char*>(_end - 1); }
        char* rend() const { return const_cast<char*>(_str - 1); }
        const char* crbegin() const { return _end - 1; }
        const char* crend() const { return _str - 1; }

        /* Get substring[start : stop]
         * Both start and stop can be negative. A negative value indicates that
         * the position is counted backward starting from the end of the string.
         * 
         * string("abc").substr(0, -1) == "ab";
         * string("abc").substr(-1) == "c";
         */
        string substr(int start = 0, int stop = INT_MAX) const {
            const char* a = _sub_checked(start);
            const char* b = _sub_checked(stop);
            return a < b ? string(a, b) : string();
        }

        /* Find the position of the first occurrence of ch in the string.
         * If ch is not found in string, npos is returned.
         */
        size_t find(char ch) const {
            const char* p = std::find(_str, _end, ch);
            return p == _end ? npos : p - _str;
        }

        /* Search string for the first occurrence of str.
         * If str is not found in string, npos is returned.
         */
        size_t search(const string& str) const {
            const char* p = std::search(_str, _end, str._str, str._end);
            return p == _end ? npos : p - _str;
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

        /* Copy the whole string to dst and returns dst.
         * copy assumes that size of buffer dst is sufficient to store the whole
         * string including a padding null character.
         */
        char* copy(char* dst) const {
            size_t mylen = length();
            memmove(dst, _str, mylen);
            dst[mylen] = 0;
            return dst;
        }

        /* Copy the initial count characters of the string to dst and returns dst.
         * If count is less than the length of the string, a null character is
         * not automatically appended to dst.
         * If count is greater than the length of the string, dst is padded with
         * null characters up to length count.
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

        /* Splits the string into multiple substrings.
         * Note that the number of substrings is always one more than the number
         * of actual splits.
         */
        std::vector<string>& split(char sep, std::vector<string>& subs, size_t maxsplits = -1) const {
            const char* a = _str;
            const char* b = a;
            for (; b < _end; ++b){
                if (*b == sep){
                    if (!--maxsplits){
                        b = _end;
                    }
                    subs.push_back(string(a, b));
                    a = b + 1;
                }
            }

            if (a != b){
                subs.push_back(string(a, b));
            }
            return subs;
        }
        
        std::vector<string>& split(const string& sep, std::vector<string>& subs, size_t maxsplits = -1) const {
            /* Separator should not be empty */
            assert(!sep.empty());

            size_t seplen = sep.length();
            const char* a = _str;
            const char* b = a;
            while (b < _end){
                b = std::search(a, _end, sep._str, sep._end);
                if (b != _end){
                    subs.push_back(string(a, b));
                    b += seplen;
                    a = b;
                }
            }

            if (a != b){
                subs.push_back(string(a, b));
            }
            return subs;
        }

        std::vector<string> split(char sep, size_t maxsplits = -1) const {
            std::vector<string> subs;
            return split(sep, subs, maxsplits);
        }
        
        std::vector<string> split(const string& sep, size_t maxsplits = -1) const {
            std::vector<string> subs;
            return split(sep, subs, maxsplits);
        }

        /* Splits the string into Key-Value pair(Requires separator string is not empty). */
        std::pair<string, string>& split_kv(char sep, std::pair<string, string>& kv) const {
            const char* p = _str;
            while (p < _end && *p != sep){
                ++p;
            }
            kv.first._assign_checked(_str, p);
            kv.second._assign_checked(p + 1, _end);
            return kv;
        }

        std::pair<string, string>& split_kv(const string& sep, std::pair<string, string>& kv) const {
            const char* p = std::search(_str, _end, sep._str, sep._end);
            kv.first._assign_checked(_str, p);
            kv.second._assign_checked(p + sep.length(), _end);
            return kv;
        }

        std::pair<string, string> split_kv(char sep) const {
            std::pair<string, string> kv;
            return split_kv(sep, kv);
        }

        std::pair<string, string> split_kv(const string& sep) const {
            std::pair<string, string> kv;
            return split_kv(sep, kv);
        }

        /* Splits the string into N parts(Requires N > 0). */
        template<size_t N>
        std::array<string, N>& split_n(char sep, std::array<string, N>& parts) const {
            assert(N > 0);

            const char* a = _str;
            const char* b = a;
            size_t n = 0;
            for (; b < _end; ++b){
                if (*b == sep){
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
        
        template<size_t N>
        std::array<string, N>& split_n(const string& sep, std::array<string, N>& parts) const {
            assert(N > 0);

            const char* a = _str;
            const char* b = a;
            size_t n = 0;
            size_t seplen = sep.length();
            while (b < _end){
                b = std::search(a, _end, sep._str, sep._end);
                if (b != _end){
                    if (N == ++n){
                        b = _end;
                    }
                    parts[n-1]._assign(a, b);
                    b += seplen;
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

        template<size_t N>
        std::array<string, N> split_n(char sep) const {
            std::array<string, N> parts;
            return split_n(sep, parts);
        }

        template<size_t N>
        std::array<string, N> split_n(const string& sep) const {
            std::array<string, N> parts;
            return split_n(sep, parts);
        }

        /* Gets the integer value represented by the string. */
        int to_int() const {
            if (*_end == 0 || *_end == ' ' || *_end == '\t'){
                return atoi(_str);
            }

            char d[16];
            return atoi(ncopy(d, 16));
        }

        /* Gets the double value represented by the string. */
        double to_double() const {
            if (*_end == 0 || *_end == ' ' || *_end == '\t'){
                return atof(_str);
            }

            char d[16];
            return atof(ncopy(d, 16));
        }

        /* Random access without checking. */
        const char& operator[](size_t i) const { return _str[i]; }
        char& operator[](size_t i) { return const_cast<char*>(_str)[i]; }

        bool operator==(const char* str) const { return equals(str); }
        bool operator==(const string& str) const { return equals(str); }
        bool operator!=(const char* str) const { return !equals(str); }
        bool operator!=(const string& str) const { return !equals(str); }

        /* Substring operator. Returns substring[start : stop] */
        string operator()(int start = 0, int stop = INT_MAX) const {
            return substr(start, stop);
        }

        static const size_t npos = (size_t)-1;

    private:
        string(const char* str, const char* end): _str(str), _end(end) {}
        
        void _assign(const char* str, const char* end){
            _str = str;
            _end = end;
        }

        const char* _sub_checked(int n) const {
            if (n >= 0){
                return (unsigned)n < length() ? _str + n : _end;
            }
            return (unsigned)-n < length() ? _end + n : _str;
        }

        void _assign_checked(const char* str, const char* end){
            if (end <= str){
                _end = _str = _zeros();
            }
            else {
                _str = str;
                _end = end;
            }
        }

        /* Buffer for an empty string */
        static const char* _zeros() {
            static int d = 0;
            return reinterpret_cast<const char*>(&d);
        }

        const char* _str;
        const char* _end;
    };

    inline std::ostream& operator<< (std::ostream& out, const string& str){
        out.write(str.buffer(), str.length());
        return out;
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
}
