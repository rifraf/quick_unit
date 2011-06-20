//
// Linux.cpp : quick_unit test
//
#include "../quick_unit.hpp"

// ----------------------------
DECLARE_SUITE(My First Tests)

TEST(the compiler can add) {
  assert(3 == 1 + 2,			SHOULD(add numbers));
}

TEST(the compiler can subtract) {
  assert_equal(11 , 2 - 1, SHOULD(subtract numbers)); // Whoops!
}

TEST(we can output text in a test) {
  Output() << "I am in test '" << name() << "'" << std::endl;
  printf("I am on line %d\n", __LINE__);
}

// ----------------------------
DECLARE_SUITE(Compiler Tests)

TEST(Compiler checks) {
  assert(__GNUC__ != 0,			 SHOULD(be true on my Linux PC));
  assert_equal(4 , __GNUC__, SHOULD(be 4 on my Linux PC));
}

// ----------------------------
#include <fstream>
#include <memory>
using namespace std;
int main(int argc, char *argv[]) {
	printf("Hello quick_unit!\n");
  RUN_TESTS();  // To default std::cout

  auto_ptr<ofstream> results_file(new ofstream("results.txt"));
  TEST_OUTPUT(*results_file);
  int fails = RUN_TESTS(); // To file results.txt
	printf("Fails: %d\n", fails);
	return fails;
}

