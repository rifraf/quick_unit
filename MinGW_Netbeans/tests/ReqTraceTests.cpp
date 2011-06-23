#include "../code_under_test/vcl.h"
#define QU_ASSERT verify
#define CAN QU_SHOULD
#include "../quick_unit.hpp"
#include "../quick_unit_req_syntax.hpp"

#include "../quick_unit_reqtrace.hpp" // EXPERIMENTAL
ADDITIONAL_REPORTER(ReqTrace)

#include "../quick_unit_netbeans.hpp"
ADDITIONAL_REPORTER(Netbeans)

//--------------------------------------------
DECLARE_SUITE(Traced AnsiString Tests)

TEST(Experiments: verify syntax) {
  verify(1,                 CAN(be true));
  verify_true(1,            CAN(be true));
  verify_false(0,           CAN(be false));
  verify_equal(1, 2-1,      CAN(be equal));
  verify_not_equal(1, 2,    CAN(differ));

  verify_equal("xy",     "x""y", CAN(be equal string));
  verify_not_equal("xy", "x""z", CAN(be different strings));
}

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
    not_equal("xy", "x""z")
  ;
}


TEST(AnsiStrings can be created: verify syntax) {
  TESTS("$VCL 1");        // EXPERIMENTAL
  TESTS("$VCL 2 : TODO"); // EXPERIMENTAL
  
  verify(new AnsiString() != NULL,            CAN(create empty AnsiString));
  verify(new AnsiString("content") != NULL,   CAN(create AnsiString with content));
  verify(new AnsiString((unsigned)5) != NULL, CAN(create AnsiString with unsigned value));
  verify(new AnsiString(-5) != NULL,          CAN(create AnsiString with signed value));
}

TEST(AnsiStrings can be created: require syntax) {
  TESTS("$VCL 11");       // EXPERIMENTAL
  TESTS("$VCL 12 : TODO");// EXPERIMENTAL
  
  REQUIRE(Create AnsiStrings)
    >> (new AnsiString() != NULL)
    >> (new AnsiString("content") != NULL)
    >> (new AnsiString((unsigned)5) != NULL)
    >> (new AnsiString(-5) != NULL)
  ;

  REQUIRE(something broken) >> FALSE;
}

TEST(AnsiString content can be read: verify syntax) {
  TESTS("$VCL 3", "$VCL 4");  // EXPERIMENTAL
  verify_equal((new AnsiString())->c_str(), "",                 CAN(be empty));
  verify_equal((new AnsiString("cotent"))->c_str(), "content",  CAN(contain 'content'));
  verify_equal((new AnsiString((unsigned)5))->c_str(), "5",     CAN(contain 5));
  verify_equal((new AnsiString(-5))->c_str(), "-5",             CAN(contain -5));
}

TEST(AnsiString content can be read: require syntax) {
  TESTS("$VCL 13", "$VCL 14");  // EXPERIMENTAL

  REQUIRE(Read content) >>
    equal(1,          2 - 1) >>
    equal("",         (new AnsiString())->c_str()) >>
    equal("conent",  (new AnsiString("content"))->c_str()) >>
    equal("5",        (new AnsiString((unsigned)5))->c_str()) >>
    equal("-5",       (new AnsiString(-5))->c_str())
  ;
}

TEST(AnsiString content can be assigned) {
  AnsiString x;
  verify_equal(x.c_str(), "",       CAN(start empty));

  x = "Hello";
  verify_equal(x.c_str(), "Hello",  CAN(be assignable from char *));

  x = std::string("Bye");
  verify_equal(x.c_str(), "Bye",    CAN(be assignable from std::string));
}

TEST(AnsiString content can be compared) {
  AnsiString x("Fi");
  AnsiString y("Fo");
  AnsiString z("Fi");

  verify(x == z,         CAN(match content));
  verify_equal(x, z,     CAN(match content));
  verify(x != y,         CAN(not match content));
  verify_not_equal(x, y, CAN(not match content));

  verify(x == "Fi",                   CAN(match string content));
  verify_equal(x.c_str(), "Fi",       CAN(match string content));
  verify(x != "Blah",                 CAN(not match string content));
  verify_not_equal(x.c_str(), "Blah", CAN(not match string content));
}

TEST(AnsiStrings can be added) {
  AnsiString x("One");
  AnsiString y("Two");

  verify_equal((x + y).c_str(), "OneTwo",           CAN(add 2 AnsiStrings));
  verify_equal((x + "Three").c_str(), "OneThree",   CAN(add an AnsiString and a string));
}

TEST(AnsiStrings can be appended) {
  AnsiString x("One");
  AnsiString y("Two");

  x += y;
  verify_equal(x.c_str(), "OneTwo",        CAN(append an AnsiString));
  x += "Three";
  verify_equal(x.c_str(), "OneTwoThree",   CAN(append a string));
}

TEST(AnsiStrings have size) {
  AnsiString x;
  verify(x.IsEmpty(),         CAN(be empty after init));
  verify_equal(0, x.Length(), CAN(be have zero length after init));

  x = "1234567890";
  verify(!x.IsEmpty(),        CAN(not be empty after assign));
  verify_equal(10, x.Length(),CAN(have length 10 chars));
}

TEST(Can find position of a substring) {
  AnsiString x("The quick brown fox");
  verify_equal( 5, x.Pos("quick"), CAN(find 'quick'));
  verify_equal( 1, x.Pos("The"),   CAN(find 'The'));
  verify_equal( 0, x.Pos("fax"),   CAN(not find 'fax'));
}

TEST(AnsiStrings can extract a sub string) {
  AnsiString x("The quick brown fox");

  AnsiString y = x.SubString(11, 5);
  verify_equal("brown", y.c_str(), CAN(extract 'brown'));

  y = x.SubString(11, 500);
  verify_equal("brown fox", y.c_str(), CAN(extract 'brown fox'));

  y = x.SubString(1, 500);
  verify_equal("The quick brown fox", y.c_str(), CAN(extract everything));
}

TEST(AnsiStrings can change case) {
  AnsiString x("Mamba #5");

  verify_equal("MAMBA #5", x.UpperCase().c_str(), CAN(return upper case));
  verify_equal("Mamba #5", x.c_str(),             CAN(not be changed));
}

TEST(AnsiStrings can parse for integers) {

  verify_equal( 3, ((AnsiString)  "").ToIntDef(3));
  verify_equal( 4, ((AnsiString)"!2").ToIntDef(4));
  verify_equal( 5, ((AnsiString) "5").ToIntDef(99));
  verify_equal( 6, ((AnsiString)"+6").ToIntDef(99));
  verify_equal(-7, ((AnsiString)"-7").ToIntDef(99));
  verify_equal( 8, ((AnsiString)" 8").ToIntDef(99));

}

TEST(AnsiStrings support printf) {
  AnsiString x;
  x.printf("One %02d %3d", 2, 3);
  verify_equal("One 02   3", x.c_str(), CAN(contain printf result));
}

//--------------------------------------------
DECLARE_SUITE(StringList Tests)

TEST(StringLists can be created) {
  verify(new TStringList() != NULL,            CAN(create empty TStringList));
}

TEST(StringLists can contain text) {
  TStringList x;
  x.Text("Some content");
  verify_equal("Some content", x.Text().c_str(), CAN(contain text));
}

TEST(Adding to StringLists gives newline separators) {
  TStringList x;

  x.Add("One");
  verify_equal("One", x.Text().c_str(), CAN(contain single element));

  x.Add("Two");
  verify_equal("One\nTwo", x.Text().c_str(), CAN(contain two elements));

  x.Add("Three");
  verify_equal("One\nTwo\nThree", x.Text().c_str(), CAN(contain Three elements));
}

TEST(A StringList can be cleared) {
  TStringList x;
  x.Text("One\nTwo\nThree");
  verify_equal("One\nTwo\nThree", x.Text().c_str(), CAN(contain nothing));

  x.Clear();
  verify_equal("", x.Text().c_str(), CAN(contain nothing));
}

TEST(A StringList has countable elements) {
  TStringList x;

  verify_equal(0, x.Count(), CAN(contain nothing));
  x.Text("One\nTwo\nThree");
  verify_equal(3, x.Count(), CAN(contain three));
}

TEST(The elements of a StringList can be accessed) {
  TStringList x;
  x.Text("One\nTwo\nThree");
  x.Add("Four");

  verify_equal("One",  x.Strings(0).c_str(), CAN(contain One));
  verify_equal("Two",  x.Strings(1).c_str(), CAN(contain Two));
  verify_equal("Three",x.Strings(2).c_str(), CAN(contain Three));
  verify_equal("Four", x.Strings(3).c_str(), CAN(contain Four));
  verify_equal("",     x.Strings(4).c_str(), CAN(contain nothing));
}

// ----------------------------
int main(int argc, char** argv) {
	return RUN_TESTS();
}

