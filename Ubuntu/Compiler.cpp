#if 0
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(__GNUC__ != 0,	SHOULD(be true on my Ubuntu PC));
  assert_equal(4 , __GNUC__, SHOULD(be 4 on my Ubuntu PC));
}
#endif