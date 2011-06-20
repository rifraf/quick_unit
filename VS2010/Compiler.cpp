//
// VS2010.cpp : quick_unit test
//

#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(_MSC_VER != 0,			    SHOULD(be true in Visual Studio 2010));
  assert_equal(1600 , _MSC_VER, SHOULD(be 1600 in Visual Studio 2010));
}