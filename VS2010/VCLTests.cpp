#include "../code_under_test/vcl.h"
#include "../code_under_test/vcl.cpp"
#include "../quick_unit.hpp"

//--------------------------------------------
DECLARE_SUITE(AnsiString Tests)

TEST(AnsiStrings can be created) {
  assert(new AnsiString() != NULL,            SHOULD(create empty AnsiString));
  assert(new AnsiString("content") != NULL,   SHOULD(create AnsiString with content));
  assert(new AnsiString((unsigned)5) != NULL, SHOULD(create AnsiString with unsigned value));
  assert(new AnsiString(-5) != NULL,          SHOULD(create AnsiString with signed value));
}

TEST(AnsiString content can be read) {
  assert_equal((new AnsiString())->c_str(), "",                  SHOULD(be empty));
  assert_equal((new AnsiString("content"))->c_str(), "content",  SHOULD(contain 'content'));
  assert_equal((new AnsiString((unsigned)5))->c_str(), "5",      SHOULD(contain 5));
  assert_equal((new AnsiString(-5))->c_str(), "-5",              SHOULD(contain -5));
}

TEST(AnsiString content can be assigned) {
  AnsiString x;
  assert_equal(x.c_str(), "",       SHOULD(start empty));

  x = "Hello";
  assert_equal(x.c_str(), "Hello",  SHOULD(be assignable from char *));

  x = std::string("Bye");
  assert_equal(x.c_str(), "Bye",    SHOULD(be assignable from std::string));
}

TEST(AnsiString content can be compared) {
  AnsiString x("Fi");
  AnsiString y("Fo");
  AnsiString z("Fi");

  assert(x == z,         SHOULD(match content));
  assert_equal(x, z,     SHOULD(match content));
  assert(x != y,         SHOULD(not match content));
  assert_not_equal(x, y, SHOULD(not match content));

  assert(x == "Fi",                   SHOULD(match string content));
  assert_equal(x.c_str(), "Fi",       SHOULD(match string content));
  assert(x != "Blah",                 SHOULD(not match string content));
  assert_not_equal(x.c_str(), "Blah", SHOULD(not match string content));
}

TEST(AnsiStrings can be added) {
  AnsiString x("One");
  AnsiString y("Two");

  assert_equal((x + y).c_str(), "OneTwo",           SHOULD(add 2 AnsiStrings));
  assert_equal((x + "Three").c_str(), "OneThree",   SHOULD(add an AnsiString and a string));
}

TEST(AnsiStrings can be appended) {
  AnsiString x("One");
  AnsiString y("Two");

  x += y;
  assert_equal(x.c_str(), "OneTwo",        SHOULD(append an AnsiString));
  x += "Three";
  assert_equal(x.c_str(), "OneTwoThree",   SHOULD(append a string));
}

TEST(AnsiStrings have size) {
  AnsiString x;
  assert(x.IsEmpty(),         SHOULD(be empty after init));
  assert_equal(0, x.Length(), SHOULD(be have zero length after init));

  x = "1234567890";
  assert(!x.IsEmpty(),        SHOULD(not be empty after assign));
  assert_equal(10, x.Length(),SHOULD(have length 10 chars));
}

TEST(Can find position of a substring) {
  AnsiString x("The quick brown fox");
  assert_equal( 5, x.Pos("quick"), SHOULD(find 'quick'));
  assert_equal( 1, x.Pos("The"),   SHOULD(find 'The'));
  assert_equal( 0, x.Pos("fax"),   SHOULD(not find 'fax'));
}

TEST(AnsiStrings can extract a sub string) {
  AnsiString x("The quick brown fox");

  AnsiString y = x.SubString(11, 5);
  assert_equal("brown", y.c_str(), SHOULD(extract 'brown'));

  y = x.SubString(11, 500);
  assert_equal("brown fox", y.c_str(), SHOULD(extract 'brown fox'));

  y = x.SubString(1, 500);
  assert_equal("The quick brown fox", y.c_str(), SHOULD(extract everything));
}

TEST(AnsiStrings can change case) {
  AnsiString x("Mamba #5");

  assert_equal("MAMBA #5", x.UpperCase().c_str(), SHOULD(return upper case));
  assert_equal("Mamba #5", x.c_str(),             SHOULD(not be changed));
}

TEST(AnsiStrings can parse for integers) {

  assert_equal( 3, ((AnsiString)  "").ToIntDef(3));
  assert_equal( 4, ((AnsiString)"!2").ToIntDef(4));
  assert_equal( 5, ((AnsiString) "5").ToIntDef(99));
  assert_equal( 6, ((AnsiString)"+6").ToIntDef(99));
  assert_equal(-7, ((AnsiString)"-7").ToIntDef(99));
  assert_equal( 8, ((AnsiString)" 8").ToIntDef(99));

}

TEST(AnsiStrings support printf) {
  AnsiString x;
  x.printf("One %02d %3d", 2, 3);
  assert_equal("One 02   3", x.c_str(), SHOULD(contain printf result));
}

//--------------------------------------------
DECLARE_SUITE(StringList Tests)

TEST(StringLists can be created) {
  assert(new TStringList() != NULL,            SHOULD(create empty TStringList));
}

TEST(StringLists can contain text) {
  TStringList x;
  x.Text("Some content");
  assert_equal("Some content", x.Text().c_str(), SHOULD(contain text));
}

TEST(Adding to StringLists gives newline separators) {
  TStringList x;

  x.Add("One");
  assert_equal("One", x.Text().c_str(), SHOULD(contain single element));

  x.Add("Two");
  assert_equal("One\nTwo", x.Text().c_str(), SHOULD(contain two elements));

  x.Add("Three");
  assert_equal("One\nTwo\nThree", x.Text().c_str(), SHOULD(contain Three elements));
}

TEST(A StringList can be cleared) {
  TStringList x;
  x.Text("One\nTwo\nThree");
  assert_equal("One\nTwo\nThree", x.Text().c_str(), SHOULD(contain nothing));

  x.Clear();
  assert_equal("", x.Text().c_str(), SHOULD(contain nothing));
}

TEST(A StringList has countable elements) {
  TStringList x;

  assert_equal(0, x.Count(), SHOULD(contain nothing));
  x.Text("One\nTwo\nThree");
  assert_equal(3, x.Count(), SHOULD(contain three));
}

TEST(The elements of a StringList can be accessed) {
  TStringList x;
  x.Text("One\nTwo\nThree");
  x.Add("Four");

  assert_equal("One",  x.Strings(0).c_str(), SHOULD(contain One));
  assert_equal("Two",  x.Strings(1).c_str(), SHOULD(contain Two));
  assert_equal("Three",x.Strings(2).c_str(), SHOULD(contain Three));
  assert_equal("Four", x.Strings(3).c_str(), SHOULD(contain Four));
  assert_equal("",     x.Strings(4).c_str(), SHOULD(contain nothing));
}
