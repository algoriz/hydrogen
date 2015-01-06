#pragma once
#include <hydrogen/common/string.h>
#include <iostream>

#include "test.h"
using namespace hy;

void string_tests() {
    BEGIN_TEST_PACKAGE("common/string");
    
    BEGIN_TEST_CASE("empty");
    {
        string e;
        TEST_CHECK(e.empty() && e.length() == 0 && e.equals(""));
    }
    END_TEST_CASE();

    BEGIN_TEST_CASE("substr");
    {
        string s("this is a short string");
        string j = s.substr(1, -1);
        TEST_CHECK(j == "his is a short strin");
        TEST_CHECK(j.buffer() == s.buffer() + 1);
        TEST_CHECK(j == s(1, -1));
        TEST_CHECK(s() == s);
        TEST_CHECK(s(3).starts_with("s is a"));
        TEST_CHECK(s(5, 7) == "is");
    }
    END_TEST_CASE();
    
    BEGIN_TEST_CASE("trim");
    {
        string d("\r\n  \ta bc \r\n\n\t  ");
        TEST_CHECK(d.trim() == "a bc");

        string b("    \r\t\n  ");
        TEST_CHECK(b.trim().empty());
    }
    END_TEST_CASE();

    BEGIN_TEST_CASE("split");
    {
        auto h = string("GET / HTTP/1.1\r\nHost: google.com\r\nContent-Length: -1\r\n\r\n").split("\r\n");
        TEST_CHECK(h.size() == 4 && h[3].empty());
        TEST_CHECK(h[1].split_kv(':').first.trim() == "Host");
        TEST_CHECK(h[1].split_kv(':').second.trim() == "google.com");
        TEST_CHECK(h[2].split_kv(':').second.to_int() == -1);
    }
    END_TEST_CASE();

    BEGIN_TEST_CASE("split_n");
    {
        auto parts = string("this is a short string").split_n<5>(' ');
        TEST_CHECK(parts.size() == 5);
        TEST_CHECK(parts[0].equals("this"));
        TEST_CHECK(parts[1].equals("is"));
        TEST_CHECK(parts[2].equals("a"));
        TEST_CHECK(parts[3].equals("short"));
        TEST_CHECK(parts[4].equals("string"));

        string("    ").split_n(' ', parts);
        TEST_CHECK(parts[0].empty() && parts[1].empty()
                   && parts[2].empty() && parts[3].empty() && parts[4].empty());

        auto addr = string("127.0.0.1").split_n<4>('.');
        TEST_CHECK(addr[0] == "127" && addr[1] == "0" && addr[2] == "0" && addr[3] == "1");
        TEST_CHECK(addr == string("127.0.0.1").split_n<4>("."));
    }
    END_TEST_CASE();
    
    BEGIN_TEST_CASE("make_lower/make_upper/starts_with/ends_with");
    {
        char str[] = "hEllO woRLd";
        auto s = string(str);
        TEST_CHECK(s.make_lower() == "hello world");
        TEST_CHECK(s.make_upper() == "HELLO WORLD");
        TEST_CHECK(s.starts_with("HELLO"));
        TEST_CHECK(!s.starts_with("HELLO-"));
        TEST_CHECK(s.ends_with("WORLD"));
        TEST_CHECK(!s.ends_with("WORLD-"));
        TEST_CHECK(string().starts_with(""));
        TEST_CHECK(string().ends_with(""));
    }
    END_TEST_CASE();

    BEGIN_TEST_CASE("to_int/to_double/to_longlong");
    {
        auto strs = string(" |0|-1|123|-1234| 10 | -10 | 777a").split('|');
        int  values[] = { 0, 0, -1, 123, -1234, 10, -10, 777 };
        size_t n = strs.size();
        for (size_t i = 0; i < n; ++i){
            TEST_CHECK(strs[i].to_int() == values[i]);
        }
    }

    {
        auto strs = string(" |0|-1|123.1|-1234.333|     10.66666666666666 | -10 | 777.001a").split('|');
        double values[] = { 0, 0, -1, 123.1, -1234.333, 10.66666666666666, -10, 777.001 };
        size_t n = strs.size();
        for (size_t i = 0; i < n; ++i){
            TEST_AEQUAL(strs[i].to_double(), values[i], 0.00000000000001);
        }
    }

    {
        auto strs = string(" |0|-1|123|-1234| 10 | -10 | 777a | 1234567890123456").split('|');
        long long  values[] = { 0, 0, -1, 123, -1234, 10, -10, 777, 1234567890123456 };
        size_t n = strs.size();
        for (size_t i = 0; i < n; ++i){
            TEST_CHECK(strs[i].to_longlong() == values[i]);
        }
    }
    END_TEST_CASE();

    //BEGIN_TEST_CASE("string_builder");
    //{
    //    char buf[1024];
    //    string_builder builder(buf, 1024);
    //    TEST_CHECK(builder.append(10).to_string().to_int() == 10);
    //    builder.clear();
    //    TEST_CHECK(builder.append(-1).to_string().to_int() == -1);
    //    builder.clear();
    //    TEST_CHECK(builder.append(123456789).to_string().to_int() == 123456789);
    //    builder.clear();
    //    TEST_CHECK(builder.append(123456789123456).to_string().to_longlong() == 123456789123456);
    //    builder.clear();
    //    TEST_CHECK(builder.append_format("hello").to_string() == "hello");
    //    TEST_CHECK(builder.append_format("").to_string() == "hello");
    //    TEST_CHECK(builder.append("").to_string() == "hello");
    //    TEST_CHECK(builder.append(" world").to_string() == "hello world");
    //    TEST_CHECK(builder.append(' ').to_string() == "hello world ");
    //    TEST_CHECK(builder.append(false).to_string() == "hello world false");
    //    TEST_CHECK(builder.append(' ').to_string() == "hello world false ");
    //    TEST_CHECK(builder.append(123456).to_string() == "hello world false 123456");
    //    builder.clear();
    //}
    //END_TEST_CASE();
    END_TEST_PACKAGE();
}
