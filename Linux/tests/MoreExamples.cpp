#include "../code_under_test/vcl.h"
#include "../quick_unit.hpp"
#include "../quick_unit_netbeans.hpp"

TEST_REPORTER(Default)
ADDITIONAL_REPORTER(Netbeans)

//--------------------------------------------
DECLARE_SUITE(Equalities)

TEST(equal) {
  assert_equal(1,   17 - 16,   SHOULD(match integers));
  assert_equal(1U,  17U - 16U, SHOULD(match unsigned integers));
  assert_equal(1.0, 2.0 / 2.0, SHOULD(match floats));

  std::string x = "One";
  std::string y = "One";
  assert_equal(x, y,           SHOULD(match strings));
}

TEST(not equal) {
  assert_not_equal(2,   17 - 16,   SHOULD(match integers));
  assert_not_equal(2U,  17U - 16U, SHOULD(match unsigned integers));
  assert_not_equal(1.0, 2.0,       SHOULD(match floats));

  std::string x = "One";
  std::string y = "Two";
  assert_not_equal(x, y,           SHOULD(match strings));
}

//--------------------------------------------
BEGIN_SUITE(Setup and teardown)
  unsigned suite_global;
  unsigned test_global;

  SETUP_SUITE    { suite_global = 3U; }
  TEARDOWN_SUITE { suite_global = 0U; }

  SETUP    { test_global = 3U; }
  TEARDOWN { test_global = 0U; }

END_SUITE_AS(suite)

TEST(First test) {
  assert_equal(3U, suite.test_global++, SHOULD(be re-initialized for each test));
  assert_equal(4U, suite.test_global++);
  assert_equal(5U, suite.test_global++);

  assert_equal(3U, suite.suite_global++, SHOULD(be initialized when suite starts));
  assert_equal(4U, suite.suite_global++);
  assert_equal(5U, suite.suite_global++);
}

TEST(Second test) {
  assert_equal(3U, suite.test_global++, SHOULD(be re-initialized for each test));
  assert_equal(4U, suite.test_global++);
  assert_equal(5U, suite.test_global++);

  assert_equal(6U, suite.suite_global++, SHOULD(persist value across tests));
  assert_equal(7U, suite.suite_global++);
  assert_equal(8U, suite.suite_global++);
}

// ----------------------------
int main(int argc, char** argv) {
	return RUN_TESTS();
}

