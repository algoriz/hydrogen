#pragma once
#include <string>
#include <cassert>
#include <cstdlib>

namespace hy {
    class string {
    public:
        /* empty string */
        string() : _str(zeros()), _end(zeros()) {}

        string(const char* str, size_t count = -1): _str(str){
            if (count == -1) {
                count = strlen(_str);
            }
            _end = _str + count;
        }

        size_t length() const { return _end - _str; }
        bool empty() const { return _end == _str; }

        /* Converts to std::string */
        std::string std_string() const {
            return std::string(_str, length());
        }

        bool c_str_compatible() const {
            return *_end == '\0';
        }

        const char* c_str() const {
            assert(c_str_compatible());
            return _str;
        }

        /* Iterators */
        const char* begin() const { return _str; }
        const char* end() const { return _end; }
        const char* rbegin() const { return _end - 1; }
        const char* rend() const { return _str - 1; }

        string substr(int start) const {
            assert(start >= 0);
            if (_str + start >= _end) {
                /* To raise an exception or not, this is a question.
                 * For now, we believe it's more often that we prefer an empty 
                 * string than an exception.
                 */
                return string();
            }
            return string(_str + start, _end);
        }

        string substr(int start, int stop) {
            assert(start >= 0);
            if (empty()) {
                return string();
            }

            const char* a = _str + start;
            const char* b = stop < 0 ? _end + stop : _str + stop;
            return a < b && b <= _end ? string(a, b) : string();
        }

        string& left_trim() {
            while (_str < _end && isspace(*_str)) {
                ++_str;
            }
            return *this;
        }

        string& right_trim() {
            while (_end > _str && isspace(_end[-1])) {
                --_end;
            }
            return *this;
        }

        string& trim() {
            return right_trim().left_trim();
        }

        int compare(const char* str) const {
            return strncmp(_str, str, length());
        }

        int compare(const string& str) const {
            size_t count = length();
            if (count > str.length()) {
                count = str.length();
            }
            return strncmp(_str, str._str, count);
        }

        int compare(const std::string& str) const {
            return compare(string(str.c_str(), str.length()));
        }

        bool equals(const char* str) const {
            return 0 == compare(str);
        }

        bool equals(const string& str) const {
            return length() == str.length()
                && 0 == memcmp(_str, str._str, length());
        }

        bool equals(const std::string& str) const {
            return length() == str.length()
                && 0 == memcmp(_str, str.c_str(), length());
        }

    private:
        string(const char* str, const char* end): _str(str), _end(end) {}
        
        /* Buffer for an empty string */
        static const int _zeros = 0;
        static const char* zeros() {
            return reinterpret_cast<const char*>(&_zeros);
        }

        const char* _str;
        const char* _end;
    };
}
