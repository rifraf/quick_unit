//
// MinGW.cpp : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(MyFirstTests)

TEST(the compiler can add) {
  assert(3 == 1 + 2,			SHOULD(add numbers));
}

TEST(the compiler can subtract) {
  assert_equal(11 , 2 - 1, SHOULD(subtract numbers));
}

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(__GNUC__ != 0,			 SHOULD(be true my MinGW));
  assert_equal(3 , __GNUC__, SHOULD(be 3 in my MinGW));
}

// ----------------------------
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
	return RUN_TESTS();
}

