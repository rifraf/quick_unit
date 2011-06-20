#include "../code_under_test/vcl.h"
#include "../quick_unit.hpp"
#include "../quick_unit_netbeans.hpp"

TEST_REPORTER(Default)
ADDITIONAL_REPORTER(Netbeans)

//--------------------------------------------
DECLARE_SUITE(Tiny Test)

TEST(Nothing much) {
  assert_equal(1, 17 - 16);
}

// ----------------------------
int main(int argc, char** argv) {
	return RUN_TESTS();
}

