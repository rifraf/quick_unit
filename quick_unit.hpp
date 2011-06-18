/* 
 * quick_unit.hpp
 * Author: David Lake
 * Description:
 *  A tiny unit testing framework for C++, intended to make it
 *  so easy to write new unit tests that you won't even have to
 *  think about it.
 * Aims:
 *  Absolute minimum ceremony while creating readable tests
 *  Single header file contains everything. Nothing else to install
 *  Cross platform
 * Example:
 *
 *  DECLARE_SUITE(MyFirstTests)
 *  
 *  TEST("the compiler can add") {
 *    assert(3 == 1 + 2,			SHOULD(add numbers));
 *  }
 *  
 *  TEST("the compiler can subtract") {
 *    assert_equal(1 , 2 - 1, SHOULD(subtract numbers));
 *  }
 *  
 *  int main(int argc, char *argv[]) {
 *  	return MyFirstTests.RunAll();
 *  }
 *
 * Inspired by Ruby. Thanks Matz. Thanks Dave Thomas.
 * Tested on:
 *  Visual Studio 2010
 *  Visual Studio 2005
 *  MinGW GCC (5.16)
 *  Debian linux
 */

/*
 * TODO:
 * - assertions for exceptions?
 * - other assertions?
 * - req tracing
 * - switchable report format (Netbeans/Human/Trace?)
 * - switchable cout
 * - decouple writer
 * Y time tests
 * Y before/after/pre/post
 */
#ifndef QUICK_UNIT_HPP
#define	QUICK_UNIT_HPP

#include <sstream>
#include <iostream>
#include <string>
#include <list>
#include <time.h>
#include <algorithm>
#include <functional>
#include <stdio.h>
#include <string.h>
#undef assert
class QUTest;
class QUTestSuite;
/******************************************************************************/
class QUTestSuiteTracker {
/******************************************************************************/

public:
  // Used to track the current QUTestSuite.
  // It is a glorified global variable.
  // QUTestSuite calls this from its constructor to set the current suite.
  // QUTests call this with a NULL parameter to get the current suite.
  static QUTestSuite *CurrentQUTestSuite(QUTestSuite *cur = NULL) {
    static QUTestSuite *current;
    if (cur) {
      current = cur;
    }
    return current;
  }
};

/******************************************************************************/
class QUTest {
/******************************************************************************/
protected:
  int _fails;
  int _passes;
  int _assertions;
  std::string _name;
  std::string _fail_message;
  std::ostringstream _info_message;
public:
  QUTest(const char *msg) {
    _fails = 0;
    _passes = 0;
    _assertions = 0;
    _fail_message = "";
    _name = msg;
  }
  virtual void Run(void) = 0;
  
  std::string name_as_token() { // Netbeans doesn't like spaces in status lines
    std::string rep(_name);
    std::replace( rep.begin(), rep.end(), ' ', '_' );
    return rep;
  }

	void assert(bool truth) {
    std::ostringstream os;
		os << "assertion #" << _assertions + 1;
    assert(truth, os.str().c_str());
	}

  void assert(bool truth, const char *msg) {
    _assertions++;
    if (truth) {
      _info_message.str("");
      _passes++;
    } else {
      _fail_message = msg;
      _fails++;
      throw _fail_message;
    }
  }

  void assert_equal(int a, int b) {
    _info_message << " (Values: " << a << "," << b << ")";
    assert(a == b);
  }
  void assert_equal(int a, int b, const char *msg) {
    _info_message << " (Values: " << a << "," << b << ")";
    assert(a == b, msg);
  }
  void assert_equal(const char *a, const char *b) {
    _info_message << " (Values: " << a << "," << b << ")";
    assert(strcmp(a,b)==0);
  }
  void assert_equal(const char *a, const char *b, const char *msg) {
    _info_message << " (Values: " << a << "," << b << ")";
    assert(strcmp(a,b)==0, msg);
  }
  
  int fails() {
    if (_passes + _fails == 0) {
      return 1; // No asserts - not a valid test
    }
    return _fails;
  }
  int passes() {return _passes;}
  std::string fail_message() {
    if (_passes + _fails == 0) {
      return "No assertions were executed";
    }
    return _fail_message + _info_message.str().c_str();
  }
};

/******************************************************************************/
class QUTestSuite {
/******************************************************************************/
private:
  int _test_id;
  std::string _suite_name;
  std::list<QUTest *> _tests;

protected:
  virtual void BeforeAll() {}  /* Callback before processing test */
  virtual void AfterAll() {}   /* Callback after processing test */
  virtual void BeforeEach() {} /* Callback before each unit test */
  virtual void AfterEach() {}  /* Callback after each unit test */

public:
  QUTestSuite(const char *msg) {
    _suite_name = msg;
    _test_id = 0;
    QUTestSuiteTracker::CurrentQUTestSuite(this);
  }
  int Add(QUTest *test) {
    _tests.push_back(test);
    return _test_id++;
  }
  
  int RunAll(void) {
    int fails = 0;
    int suite_start, suite_end;
    suite_start = clock();
    std::cout << "%SUITE_STARTING% " << _suite_name << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;
    BeforeAll();
    for (std::list<QUTest *>::iterator iter = _tests.begin(); iter != _tests.end(); ++iter) {
      bool failed = false;
      std::cout << std::endl << "%TEST_STARTED% " << (*iter)->name_as_token() << " (" << _suite_name << ")" << std::endl;
      int test_start = clock();
      BeforeEach();
      try {
        (*iter)->Run();
      } catch(std::string msg) {
        std::cout << msg << std::endl;
      } catch(...) {
        failed = true;
      }
      AfterEach();
      int test_end = clock();
      double diff = (test_end - test_start) / 1000.0;
      if (failed || (*iter)->fails()) {
        fails++;
        std::cout <<
          "%TEST_FAILED% time=" << std::fixed << diff << " testname=" <<
          (*iter)->name_as_token() <<
          " (" << _suite_name << ") message=" << (*iter)->fail_message() << std::endl;
      }
      std::cout << "%TEST_FINISHED% time=" << std::fixed << diff << " " << (*iter)->name_as_token() << " (" << _suite_name << ")" << std::endl;
    }
    AfterAll();
    suite_end = clock();
    double diff = (suite_end - suite_start) / 1000.0;
    std::cout << std::endl << "%SUITE_FINISHED% time=" << std::fixed << diff << std::endl;
    return fails;
  }
};

/******************************************************************************/
/* Macros for creating a TEST */
#define _UNIQ_ID_2(x,y) x##y
#define _UNIQ_ID_1(x,y) _UNIQ_ID_2(x,y)
#define _UNIQ_ID_(x) _UNIQ_ID_1(x,  __LINE__ )

// MUST be on a single line
#define TEST(message) \
namespace  { class _UNIQ_ID_(QUTest) : public QUTest {public: _UNIQ_ID_(QUTest)(const char *name) : QUTest(name) {QUTestSuiteTracker::CurrentQUTestSuite()->Add(this);} void Run(void); } static _UNIQ_ID_(test)(message);} void _UNIQ_ID_(QUTest)::Run(void)

/******************************************************************************/
/* Macros for creating a SHOULD message */
#define _LINE_PASTE2(x,y) x#y
#define _LINE_PASTE(x,y) _LINE_PASTE2(x,y)
#define SHOULD(msg) _LINE_PASTE("line ",__LINE__) ": Should " # msg

/******************************************************************************/
/* Macros for creating a SUITE */
#define BEGIN_SUITE(name) \
class C##name : public QUTestSuite{\
  public: C##name(const char *n) : QUTestSuite(n) {}
#define BEFORE_ALL void BeforeAll()
#define AFTER_ALL void AfterAll()
#define BEFORE_EACH void BeforeEach()
#define AFTER_EACH void AfterEach()
#define END_SUITE(name) \
} static name(# name);

#define DECLARE_SUITE(name) BEGIN_SUITE(name) END_SUITE(name)

#endif	/* QUICK_UNIT_HPP */
