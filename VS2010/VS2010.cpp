//
// VS2010.cpp : quick_unit test
//

#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(My First Tests)

TEST(the compiler can add) {
  assert(3 == 1 + 2,			SHOULD(add numbers));
}

TEST(the compiler can subtract) {
  assert_equal(11 , 2 - 1, SHOULD(subtract numbers));
}

// ----------------------------
DECLARE_SUITE(CompilerTests)

TEST(Compiler checks) {
  assert(_MSC_VER != 0,			    SHOULD(be true in Visual Studio 2010));
  assert_equal(1600 , _MSC_VER, SHOULD(be 1600 in Visual Studio 2010));
}

extern int RunVCLTests(void);
// ----------------------------
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
	return RUN_TESTS() + RunVCLTests();
}


