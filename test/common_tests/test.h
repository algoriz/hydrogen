#pragma once

#define BEGIN_TEST_PACKAGE(package_name)\
do {  const char* _hydrogen_test_package_name = package_name;\
      int _hydrogen_test_package_total_case = 0;\
      int _hydrogen_test_package_failed_case = 0;\
      std::cout << "==== Package [" << package_name << "] ====\n";

#define BEGIN_TEST_CASE(case_name)\
    ++_hydrogen_test_package_total_case;\
    do { const char* _hydrogen_test_case_name = case_name;\
         bool _hydrogen_test_continue = true;\
         bool _hydrogen_test_failed = false;


#define TEST_CHECK(expr)\
      while (_hydrogen_test_continue && !(expr)){\
        std::cout << "FAILED Case " << _hydrogen_test_case_name \
                  << ", Source expression: " << #expr << '\n';\
        _hydrogen_test_failed = true;\
        break;\
      }


#define TEST_ASSERT(expr)\
      while (_hydrogen_test_continue && !(expr)){\
        std::cout << "**** FATAL ERROR Case " << _hydrogen_test_case_name \
                  << "****, Source expression: " << #expr << '\n';\
        _hydrogen_test_continue = false;\
        _hydrogen_test_failed = true;\
        std::cout << "Test case [" << _hydrogen_test_case_name << "] ABORTED!!!\n";\
        break;\
      }


#define END_TEST_CASE()\
        if (_hydrogen_test_failed){ ++_hydrogen_test_package_failed_case; }\
        std::cout << "Case [" << _hydrogen_test_case_name << "] " << (_hydrogen_test_failed ? "FAILED" : "PASSED") << '\n';\
      } while (false)


#define END_TEST_PACKAGE()\
    int _hydrogen_test_package_passed_case =_hydrogen_test_package_total_case -  _hydrogen_test_package_failed_case;\
    std::cout << "==== Package [" << _hydrogen_test_package_name << "]: ";\
    std::cout << _hydrogen_test_package_total_case << " total, "\
              << _hydrogen_test_package_passed_case << " PASSED, "\
              << _hydrogen_test_package_failed_case << " FAILED, "\
              << "PR = " << (_hydrogen_test_package_passed_case * 100 / _hydrogen_test_package_total_case) << "% ====\n\n";\
} while (false)
