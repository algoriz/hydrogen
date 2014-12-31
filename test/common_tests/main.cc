#include <iostream>

#define TEST(mod) do { void mod##_tests();\
    std::cout << "Running " << #mod << "_tests()...\n";\
    mod##_tests();\
  } while (false)

int main(int argc, char* argv[]) {
    TEST(string);
    return 0;
}

