#include <iostream>

#define TEST(libname) do {\
    int libname##_tests(); int n = libname##_tests(); \
    std::cout << ">>>> Testing " << #libname << " ... "; \
    if (n){ std::cout << "FAILED at case#" << n << " ****\n";}\
        else { std::cout << "PASSED\n"; }\
    } while (false)


int main(int argc, char* argv[]) {
    TEST(string);
    return 0;
}
