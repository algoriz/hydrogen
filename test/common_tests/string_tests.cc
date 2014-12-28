#pragma once
#include <hydrogen/common/string.h>

using namespace hy;

bool string_tests() {
    {
        // empty string
        string e;
        if (!e.empty() || !e.c_str_compatible() || !e.equals("")) {
            return false;
        }

        string t = e.substr(1);
        t = t.substr(0, -1);
        if (!t.equals(e)) {
            return false;
        }
    }

    {
        string s("this is a short string");
        std::string str(s.c_str());
        if (s.length() != str.length() || s.compare(str) != 0) {
            return false;
        }
    }
    return true;
}

