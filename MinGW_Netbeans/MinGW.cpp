//
// MinGW.cpp : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(MyFirstTests)

TEST("the compiler can add") {
  assert(3 == 1 + 2,			SHOULD(add numbers));
}

TEST("the compiler can subtract") {
  assert_equal(1 , 2 - 1, SHOULD(subtract numbers));
}

// ----------------------------
DECLARE_SUITE(CompilerTests)

TEST("Compiler checks") {
  assert(__GNUC__ != 0,			 SHOULD(be true my MinGW));
  assert_equal(3 , __GNUC__, SHOULD(be 3 in my MinGW));
}

extern int RunAnsiStringTests(void);
// ----------------------------
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
	return MyFirstTests.RunAll() + CompilerTests.RunAll();
}

