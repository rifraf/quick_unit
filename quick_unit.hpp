/* 
 * quick_unit.hpp : http://github.com/rifraf/quick_unit
 * Author: David Lake
 *
 * Description:
 *  A tiny unit testing framework for C++, intended to make it
 *  so easy to write new unit tests that you won't even have to
 *  think about it.
 *
 * Aims:
 *  Absolute minimum ceremony while creating readable tests
 *  Single header file contains everything. Nothing else to install
 *  Cross platform
 *
 * Example:
 *
 *  DECLARE_SUITE(My First Tests)
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
 *  	return RUN_TESTS();
 *  }
 *
 * Inspired by Ruby. Thanks Matz. Thanks Dave Thomas.
 *
 * Advanced features:
 *  Plugin architecture for reporting allows you to replace
 *  the default output text formatter. Alternatively you can
 *  run additional reporters at the same time. See GitHub Wiki.
 *
 * Tested on:
 *  Visual Studio 2010
 *  Visual Studio 2005
 *  MinGW GCC (5.16)
 *  Debian linux
 */

/*
 * TODO:
 * - switchable cout
 * - req tracing
 * - document reporters
 * - document suite callbacks
 * - assertions for exceptions?
 * - other assertions? templated assertions?
 * Y remove duplication in suite naming
 * Y chainable reporters
 * Y switchable report format (Netbeans/Human/Trace?)
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
protected:
  QUReporter *_chain;
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

  QUReporter() {_chain = NULL; }  
  void chain(QUReporter *chain) { _chain = chain; }
  QUReporter *chain(void) {return _chain; }

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
  // QUTestSuite calls this from its constructor to set the current suite and
  // get the old one.
  // QUTests call this with a NULL parameter to get the current suite.
  static QUTestSuite *CurrentQUTestSuite(QUTestSuite *cur = NULL) {
    static QUTestSuite *current;
    if (cur) {
      QUTestSuite *old_cur = current;
      current = cur;
      return old_cur;
    }
    return current;
  }

  // Used to track the current QUReporter, and to set it to the
  // default reporter if no other reporter is declared.
  static QUReporter *CurrentQUReporter(QUReporter *cur = NULL, bool chain = false) {
    static DefaultReporter defaultQUReporter;
    static QUReporter *current;
    if (!current) {
      current = &defaultQUReporter;
    }
    if (cur) {
      if (chain) {
        cur->chain(current);
      }
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
    _info_message << " (Expected: " << a << ", got: " << b << ")";
    assert(a == b);
  }
  void assert_equal(int a, int b, const char *msg) {
    _info_message << " (Expected: " << a << ", got: " << b << ")";
    assert(a == b, msg);
  }
  void assert_equal(const char *a, const char *b) {
    _info_message << " (Expected: " << a << ", got: " << b << ")";
    assert(strcmp(a,b)==0);
  }
  void assert_equal(const char *a, const char *b, const char *msg) {
    _info_message << " (Expected: " << a << ", got: " << b << ")";
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
  QUTestSuite * _chain;

protected:
  virtual void BeforeAllTests() {}
  virtual void AfterAllTests() {}
  virtual void BeforeEachTest() {}
  virtual void AfterEachTest() {}

public:
  QUTestSuite(const char *msg) {
    _suite_name = msg;
    _reporter = QUTestSuiteTracker::CurrentQUReporter();
    _chain = QUTestSuiteTracker::CurrentQUTestSuite(this);
  }
  void Add(QUTest *test) {
    _tests.push_back(test);
  }  
  int RunAll(void) {
    if (_chain) {
      _chain->RunAll();
    }
    std::list<QUReporter *> reporters;
    QUReporter *r = _reporter;
    while(r) {
      reporters.push_back(r);
      r = r->chain();
    }
    unsigned fails = 0;
    unsigned passes = 0;
    int suite_start = clock();

    #define EACH_QUREPORTER(op) for (std::list<QUReporter *>::iterator qfiter = reporters.begin(); qfiter != reporters.end(); ++qfiter) {(*qfiter)->op; }
    #define EACH_QUREPORTER_REVERSE(op) for (std::list<QUReporter *>::reverse_iterator qriter = reporters.rbegin(); qriter != reporters.rend(); ++qriter) {(*qriter)->op; }
    EACH_QUREPORTER(StartingSuite(_suite_name))
    BeforeAllTests();
		EACH_QUREPORTER(StartedSuite(_suite_name))
    for (std::list<QUTest *>::iterator iter = _tests.begin(); iter != _tests.end(); ++iter) {
      bool failed = false;
      std::string test_name = (*iter)->name();
			EACH_QUREPORTER(StartingTest(_suite_name, test_name))
      int test_start = clock();
      BeforeEachTest();
			EACH_QUREPORTER(StartedTest(_suite_name, test_name))
      try {
        (*iter)->Run();
      } catch(QUTestFail) {
        // Failed assertions cause us to come here
      } catch(...) {
        failed = true;
      }
			EACH_QUREPORTER_REVERSE(StoppingTest(_suite_name, test_name))
      AfterEachTest();
      double duration = (clock() - test_start) / 1000.0;
      if (failed || (*iter)->fails()) {
        fails++;
				EACH_QUREPORTER_REVERSE(FailedTest(_suite_name, test_name, duration, (*iter)->fail_message()))
      } else {
				passes++;
				EACH_QUREPORTER_REVERSE(PassedTest(_suite_name, test_name, duration))
			}
			EACH_QUREPORTER_REVERSE(CompletedTest(_suite_name, test_name, duration))
    }
		EACH_QUREPORTER_REVERSE(StoppingSuite(_suite_name))
    AfterAllTests();
		EACH_QUREPORTER_REVERSE(CompletedSuite(_suite_name, (clock() - suite_start) / 1000.0, passes, fails))
    return fails;
  }
};

/******************************************************************************/
/* Macros for creating a TEST */
#define _UNIQ_ID_2(x,y) x##y
#define _UNIQ_ID_1(x,y) _UNIQ_ID_2(x,y)
#define _UNIQ_ID_(x) _UNIQ_ID_1(x,  __LINE__ )

// MUST be on a single line
#define TEST(name) \
namespace  { class _UNIQ_ID_(QUTest) : public QUTest {public: _UNIQ_ID_(QUTest)() : QUTest(#name) {QUTestSuiteTracker::CurrentQUTestSuite()->Add(this);} void Run(void); } static _UNIQ_ID_(test);} void _UNIQ_ID_(QUTest)::Run(void)

/******************************************************************************/
/* Macros for creating a SHOULD message */
#define _LINE_PASTE2(x,y) x#y
#define _LINE_PASTE(x,y) _LINE_PASTE2(x,y)
#define SHOULD(msg) _LINE_PASTE("line ",__LINE__) ": Should " # msg

/******************************************************************************/
/* Macros for creating a SUITE */
#define BEGIN_SUITE(name) \
using namespace quick_unit; \
class _UNIQ_ID_(QUSuite) : public QUTestSuite{ public: _UNIQ_ID_(QUSuite)() : QUTestSuite(#name) {}
#define BEFORE_ALL void BeforeAllTests()
#define AFTER_ALL void AfterAllTests()
#define BEFORE_EACH void BeforeEachTest()
#define AFTER_EACH void AfterEachTest()
#define END_SUITE \
} static _UNIQ_ID_(QUSuite);

#define DECLARE_SUITE(name) BEGIN_SUITE(name) END_SUITE

#define RUN_TESTS() QUTestSuiteTracker::CurrentQUTestSuite()->RunAll()
/******************************************************************************/
/* Macros for REPORTERs */
#define TEST_REPORTER(name) \
using namespace quick_unit; namespace  { class _UNIQ_ID_(QUReporter) : public name ##Reporter { public: _UNIQ_ID_(QUReporter)() { QUTestSuiteTracker::CurrentQUReporter(this);} } static _UNIQ_ID_(Reporter); }

#define ADDITIONAL_REPORTER(name) \
using namespace quick_unit; namespace  { class _UNIQ_ID_(QUReporter) : public name ##Reporter { public: _UNIQ_ID_(QUReporter)() { QUTestSuiteTracker::CurrentQUReporter(this, true);} } static _UNIQ_ID_(Reporter); }

#define BEGIN_REPORTER(name) namespace quick_unit { class name ##Reporter : public QUReporter { public:
#define END_REPORTER() }; }

} /* namespace */
#endif	/* QUICK_UNIT_HPP */
