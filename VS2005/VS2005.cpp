//
// VS2005.cpp : quick_unit test
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
  assert(_MSC_VER != 0,			    SHOULD(be true in Visual Studio 2005));
  assert_equal(1400 , _MSC_VER, SHOULD(be 1600 in Visual Studio 2005));
}

extern int RunVCLTests(void);
// ----------------------------
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
	return MyFirstTests.RunAll() + CompilerTests.RunAll() + RunVCLTests();
}

