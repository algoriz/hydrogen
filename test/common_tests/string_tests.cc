#pragma once
#include <hydrogen/common/string.h>
#include <iostream>

using namespace hy;

#define REQUIRES(expr, c) if (!(expr)){ return c; }

int string_tests() {
    {
        // empty string
        string e;
        REQUIRES(e.empty() || e.length() != 0 || !e.equals(""), 1);

        string t = e.substr(1);
        t = t.substr(0, -1);
        REQUIRES(t.equals(e), 2);
    }

    {
        string s("this is a short string");
        if (s.length() != s.std_string().length() || s.std_string() != s.buffer()) {
            return 3;
        }

        // split_n
        auto parts = string("this is a short string").split_n<5>(' ');
        REQUIRES(parts.size() == 5, 4);
        REQUIRES(parts[0].equals("this"), 4);
        REQUIRES(parts[1].equals("is"), 4);
        REQUIRES(parts[2].equals("a"), 4);
        REQUIRES(parts[3].equals("short"), 4);
        REQUIRES(parts[4].equals("string"), 4);
    
        string("    ").split_n(' ', parts);
        if (!parts[0].empty() || !parts[1].empty()
            || !parts[2].empty() || !parts[3].empty() || !parts[4].empty()){
            return 5;
        }

        auto addr = string("127.0.0.1").split_n<4>('.');
        if (addr[0] != "127" || addr[1] != "0" || addr[2] != "0" || addr[3] != "1"
            || addr != string("127.0.0.1").split_n<4>(".")){
            return 6;
        }

        // split_kv, trim, to_int
        auto kv = string("Content-Length: 12\r\n").split_kv(':');
        auto kv2 = string("Content-Length: 12\r\n").split_kv(":");
        kv.first.trim();
        kv.second.trim();
        kv2.first.trim();
        kv2.second.trim();
        REQUIRES(kv == kv2, 8);
        REQUIRES(kv.first == "Content-Length", 8);
        REQUIRES(kv.second == "12", 8);
        REQUIRES(kv.second.to_int() == 12, 8);
    }

    {
        // string split
        auto h = string("GET / HTTP/1.1\r\nHost: google.com\r\nContent-Length: -1\r\n\r\n").split("\r\n");

        if (h.size() != 4
            || !h[3].empty()
            || h[2].split_kv(':').second.to_int() != -1
            || h[1].split_kv(':').first.trim() != "Host"){
            return 9;
        }
    }

    {
        // make_lower, make_upper, starts_with, ends_with
        char str[] = "hEllO woRLd";
        auto s = string(str);
        REQUIRES(s.make_lower() == "hello world", 10);
        REQUIRES(s.make_upper() == "HELLO WORLD", 10);
        REQUIRES(s.starts_with("HELLO"), 10);
        REQUIRES(!s.starts_with("HELLO-"), 10);
        REQUIRES(s.ends_with("WORLD"), 10);
        REQUIRES(!s.ends_with("WORLD-"), 10);

        REQUIRES(string().starts_with(""), 10);
        REQUIRES(string().ends_with(""), 10);
    }
    return 0;
}

