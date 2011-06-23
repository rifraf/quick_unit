//
// RequireSyntax.cpp: Checking that we can use REQUIRE() >>
//

#include "../quick_unit.hpp"
#include "../quick_unit_req_syntax.hpp"
#include "../quick_unit_netbeans.hpp"

// ----------------------------
DECLARE_SUITE(Require syntax)

TEST(Experiments: require syntax) {

  REQUIRE(First Experiment) >> true;

  REQUIRE(Second Experiment) >>
    1 >>
    !0 >>
    is_true(1) >>
    is_false(0) >>
    equal(1, 2-1) >>
    not_equal(1,2) >>
    equal("xy", "x""y") >>
    not_equal("xy", "x""z");
}

