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
 * - other assertions? templated assertions?
 * - req tracing
 * Y switchable report format (Netbeans/Human/Trace?)
 * - switchable cout
 * Y decouple writer
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

namespace quick_unit {
class QUTest;
class QUTestSuite;

/******************************************************************************/
class QUReporter {  // Base class for all test reporters
/******************************************************************************/
public:
	virtual void StartingSuite(const std::string &suite_name) {} // Before anything
	virtual void StartedSuite(const std::string &suite_name) {}  // Just after BeforeAllTests();
	virtual void StoppingSuite(const std::string &suite_name) {} // Before AfterAllTests();
	virtual void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {} // After AfterAllTests();

	virtual void StartingTest(const std::string &suite_name, const std::string &test_name) {} // Before anything
	virtual void StartedTest(const std::string &suite_name, const std::string &test_name) {}  // Just after BeforeEachTest();
	virtual void StoppingTest(const std::string &suite_name, const std::string &test_name) {} // Before AfterEachTest();
	virtual void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {} // After AfterEachTest();
	virtual void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {} // After AfterEachTest();
	virtual void CompletedTest(const std::string &suite_name, const std::string &test_name, double duration) {} // After AfterEachTest();

  // Helper: returns the current date/time
	static const char *current_time(void) {
    time_t szClock;
    time( &szClock );
		#ifdef _MSC_VER
		static char timebuf[26];
		struct tm newtime;
		localtime_s(&newtime, &szClock);
    asctime_s(timebuf, 26, &newtime);
		return timebuf;
		#else
    return asctime(localtime(&szClock));
		#endif
	}
};

/******************************************************************************/
class DefaultReporter : public QUReporter { // The default test reporter
/******************************************************************************/
public:
	virtual void StartingSuite(const std::string &suite_name) {
    std::cout << std::endl << "----------------------------------------------------" << std::endl << "Starting " << suite_name << " at " << QUReporter::current_time() << std::endl;
	}
	virtual void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    std::cout << std::endl << "====================================================" << std::endl << "Finished " << suite_name << " at " << QUReporter::current_time() <<
			"Passes: " << passes << " Fails: " << fails << std::endl;
	}
	virtual void StartingTest(const std::string &suite_name, const std::string &test_name) {
    std::cout << "Test: " << test_name << " => ";
	}
	virtual void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
		std::cout << "FAILED. " << fail_message << std::endl;
	}
	virtual void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {
		std::cout << "OK." << std::endl;
	}
};

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

  // Used to track the current QUReporter, and to set it to the
  // default reporter if no other reporter is declared.
  static QUReporter *CurrentQUReporter(QUReporter *cur = NULL) {
    static DefaultReporter defaultQUReporter;
    static QUReporter *current;
    if (!current) {
      current = &defaultQUReporter;
    }
    if (cur) {
      current = cur;
    }
    return current;
  }
};

/******************************************************************************/
class QUTestFail {
/******************************************************************************/
  // Exception object thrown when a test fails
};

/******************************************************************************/
class QUTest {  // Pure base class for all tests
/******************************************************************************/
protected:
  int _fails;
  int _passes;
  int _assertions;
  std::string _test_name;
  std::string _fail_message;
  std::ostringstream _info_message;
  std::string _full_message;
public:
  QUTest(const char *msg) {
    _fails = 0;
    _passes = 0;
    _assertions = 0;
    _fail_message = "";
    _test_name = msg;
  }
  virtual void Run(void) = 0;

  const std::string &name() {
    return _test_name;
  }

  int passes() {
    return _passes;
  }
  
  int fails() {
    if (_passes + _fails == 0) {
      return 1; // No asserts - not a valid test. Report as a fail
    }
    return _fails;
  }

  const std::string &fail_message() {
    if (_passes + _fails == 0) {
			_full_message = "No assertions were executed";
    } else {
			_full_message = _fail_message + _info_message.str();
		}
    return _full_message;
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
      throw new QUTestFail();
    }
  }

  void assert_equal(int a, int b) {
    _info_message << " (Expected: " << a << ", got " << b << ")";
    assert(a == b);
  }
  void assert_equal(int a, int b, const char *msg) {
    _info_message << " (Expected: " << a << ", got " << b << ")";
    assert(a == b, msg);
  }
  void assert_equal(const char *a, const char *b) {
    _info_message << " (Expected: " << a << ", got " << b << ")";
    assert(strcmp(a,b)==0);
  }
  void assert_equal(const char *a, const char *b, const char *msg) {
    _info_message << " (Expected: " << a << ", got " << b << ")";
    assert(strcmp(a,b)==0, msg);
  }
};

/******************************************************************************/
class QUTestSuite {
/******************************************************************************/
private:
  std::string _suite_name;
  std::list<QUTest *> _tests;
	QUReporter * _reporter;

protected:
  virtual void BeforeAllTests() {}
  virtual void AfterAllTests() {}
  virtual void BeforeEachTest() {}
  virtual void AfterEachTest() {}

public:
  QUTestSuite(const char *msg) {
    _suite_name = msg;
    _reporter = QUTestSuiteTracker::CurrentQUReporter();
    QUTestSuiteTracker::CurrentQUTestSuite(this);
  }
  void Add(QUTest *test) {
    _tests.push_back(test);
  }  
  int RunAll(void) {
    unsigned fails = 0;
    unsigned passes = 0;
    int suite_start = clock();
		_reporter->StartingSuite(_suite_name);
    BeforeAllTests();
		_reporter->StartedSuite(_suite_name);
    for (std::list<QUTest *>::iterator iter = _tests.begin(); iter != _tests.end(); ++iter) {
      bool failed = false;
      std::string test_name = (*iter)->name();
			_reporter->StartingTest(_suite_name, test_name);
      int test_start = clock();
      BeforeEachTest();
			_reporter->StartedTest(_suite_name, test_name);
      try {
        (*iter)->Run();
      } catch(QUTestFail fail) {
        // Failed assertions cause us to come here
      } catch(...) {
        failed = true;
      }
			_reporter->StoppingTest(_suite_name, test_name);
      AfterEachTest();
      double duration = (clock() - test_start) / 1000.0;
      if (failed || (*iter)->fails()) {
        fails++;
				_reporter->FailedTest(_suite_name, test_name, duration, (*iter)->fail_message());
      } else {
				passes++;
				_reporter->PassedTest(_suite_name, test_name, duration);
			}
			_reporter->CompletedTest(_suite_name, test_name, duration);
    }
		_reporter->StoppingSuite(_suite_name);
    AfterAllTests();
		_reporter->CompletedSuite(_suite_name, (clock() - suite_start) / 1000.0, passes, fails);
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
using namespace quick_unit; \
class QUSuite##name : public QUTestSuite{\
  public: QUSuite##name(const char *n) : QUTestSuite(n) {}
#define BEFORE_ALL void BeforeAllTests()
#define AFTER_ALL void AfterAllTests()
#define BEFORE_EACH void BeforeEachTest()
#define AFTER_EACH void AfterEachTest()
#define END_SUITE(name) \
} static name(# name);

#define DECLARE_SUITE(name) BEGIN_SUITE(name) END_SUITE(name)

/******************************************************************************/
/* Macros for REPORTERs */
#define TEST_REPORTER(name) \
using namespace quick_unit; namespace  { class _UNIQ_ID_(QUReporter) : public name ##Reporter { public: _UNIQ_ID_(QUReporter)() { QUTestSuiteTracker::CurrentQUReporter(this);} } static _UNIQ_ID_(Reporter); }

#define BEGIN_REPORTER(name) namespace quick_unit { class name ##Reporter : public QUReporter { public:
#define END_REPORTER() }; }

} /* namespace */
#endif	/* QUICK_UNIT_HPP */
