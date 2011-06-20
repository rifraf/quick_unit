#if 1
//
// Ubuntu : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(My First Tests)

TEST(the compiler can add) {
  assert(3 == 1 + 2, SHOULD(add numbers));
}

TEST(the compiler can multiply) {
  assert_equal(49, 7 * 7, SHOULD(multiply numbers));
}

// ----------------------------
int main(int argc, char *argv[]) {
  printf("Hello quick_unit!\n");

  return RUN_TESTS();
}

#endif 