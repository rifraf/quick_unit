//
// MinGW.cpp : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(__GNUC__ != 0,      SHOULD(be true for my MinGW));
  assert_equal(3 , __GNUC__, SHOULD(be 3 in my MinGW));
}
