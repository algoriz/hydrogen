#include <iostream>

#define TEST(libname) do {\
    bool libname##_tests();\
    std::cout << ">>>> Testing " << #libname << " ...\n"; \
    std::cout << (libname##_tests() ? "PASSED" : "**** FAILED ****") << '\n';} while (false)


int main(int argc, char* argv[]) {
    TEST(string);
    return 0;
}
