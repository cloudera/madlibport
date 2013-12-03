
#include <iostream>
// Macros for running tests
#define RUNTEST(blah) \
    if (blah()) { printf("%-10s " # blah "\n", "ok"); } \
    else { printf("%-10s " # blah "\n", "FAIL**"); }

#define EXPECT_EQ(a, b) \
    if ((a) != (b)) { \
      std::cout << "Expected (" # b ") " << (b) << "   got (" # a ") " << (a) << std::endl; \
      return 0; \
    }

#define EXPECT_NEAR(a, b, tol) \
    if ( std::abs(a - b) > tol) { \
      std::cout << "Expected (" # b ") " << b << "   got (" # a ") " << a << std::endl; \
      return 0; \
    }

#define RUN_TEST_SET(name) \
    printf("    --------- " # name "\n"); \
    name();

#define DP(foo) ((double*)foo)
