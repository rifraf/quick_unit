//
// Linux.cpp : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(My First Tests)

TEST(the compiler can add) {
  assert(3 == 1 + 2, SHOULD(add numbers));
}

TEST(the compiler can subtract) {
  assert_equal(11 , 2 - 1, SHOULD(subtract numbers));
}

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(__GNUC__ != 0,			 SHOULD(be true on my Linux PC));
  assert_equal(4 , __GNUC__, SHOULD(be 4 on my Linux PC));
}

// ----------------------------
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
	return RUN_TESTS();
}

