//
// VS2005.cpp : quick_unit test
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
