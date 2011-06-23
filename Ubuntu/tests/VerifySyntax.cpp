//
// VerifySyntax.cpp: Checking that we can change 'assert' to something else
//

#define QU_ASSERT verify
#define CAN QU_SHOULD
#include "../quick_unit.hpp"
#include "../quick_unit_netbeans.hpp"

// ----------------------------
DECLARE_SUITE(Verify syntax)

TEST(Experiments: verify syntax) {
  verify(1,                 CAN(be true));
  verify_true(1,            CAN(be true));
  verify_false(0,           CAN(be false));
  verify_equal(1, 2-1,      CAN(be equal));
  verify_not_equal(1, 2,    CAN(differ));

  verify_equal("xy",     "x""y", CAN(be equal string));
  verify_not_equal("xy", "x""z", CAN(be different strings));
}
