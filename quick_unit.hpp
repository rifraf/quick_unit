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
  std::string _full_message;
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
      return 1; // No asserts - not a valid test. Report as a fail
    }
    return _fails;
  }
  int passes() {return _passes;}
  const std::string &fail_message() {
    if (_passes + _fails == 0) {
			_full_message = "No assertions were executed";
    } else {
			_full_message = _fail_message + _info_message.str();
		}
    return _full_message;
  }
};

class QUReporter {
public:
	// Before anything
	virtual void StartingSuite(const std::string &suite_name) {
    std::cout << std::endl << "----------------------------------------------------" << std::endl << "Starting " << suite_name << " at " << QUReporter::current_time() << std::endl;
	}
	// Just after BeforeAll();
	virtual void StartedSuite(const std::string &suite_name) {}
	// Before AfterAll();
	virtual void StoppingSuite(const std::string &suite_name) {}
	// After AfterAll();
	virtual void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    std::cout << std::endl << "====================================================" << std::endl << "Finished " << suite_name << " at " << QUReporter::current_time() << 
			"Passes: " << passes << " Fails: " << fails << std::endl;
	}

	// Before anything
	virtual void StartingTest(const std::string &suite_name, const std::string &test_name) {
    std::cout << test_name << ": ";
	}
	// Just after BeforeEach();
	virtual void StartedTest(const std::string &suite_name, const std::string &test_name) {}
	// Before AfterEach();
	virtual void StoppingTest(const std::string &suite_name, const std::string &test_name) {}
	// After AfterEach();
	virtual void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
		std::cout << "FAILED. " << fail_message << std::endl;
	}
	// After AfterEach();
	virtual void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {
		std::cout << "OK." << std::endl;
	}
	// After AfterEach();
	virtual void CompletedTest(const std::string &suite_name, const std::string &test_name, double duration) {}

	static const char *current_time(void) {
    time_t      szClock;
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

class QUNetbeansReporter : public QUReporter {
public:
	virtual void StartingSuite(const std::string &suite_name) { std::cout << "%SUITE_STARTING% " << suite_name << std::endl; }
	virtual void StartedSuite(const std::string &suite_name)  { std::cout << "%SUITE_STARTED%" << std::endl;}
	virtual void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    std::cout << std::endl << "%SUITE_FINISHED% time=" << std::fixed << duration << std::endl;
	}
	virtual void StartedTest(const std::string &suite_name, const std::string &test_name) {
		std::cout << "%TEST_STARTED% " << test_name << " (" << suite_name << ")" << std::endl;
	}
	virtual void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
		std::cout << "%TEST_FAILED% time=" << std::fixed << duration << " testname=" << test_name << " (" << suite_name << ") message=" << fail_message << std::endl;
	}
  virtual void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {}
	virtual void CompletedTest(const std::string &suite_name, const std::string &test_name, double duration) {
		std::cout << "%TEST_FINISHED% time=" << std::fixed << duration << " " << test_name << " (" << suite_name << ")" << std::endl;
	}
};

/******************************************************************************/
class QUTestSuite {
/******************************************************************************/
private:
  int _test_id;
  std::string _suite_name;
  std::list<QUTest *> _tests;
	QUReporter * _reporter;

protected:
  virtual void BeforeAll() {}  /* Callback before processing test */
  virtual void AfterAll() {}   /* Callback after processing test */
  virtual void BeforeEach() {} /* Callback before each unit test */
  virtual void AfterEach() {}  /* Callback after each unit test */

public:
  QUTestSuite(const char *msg) {
    _suite_name = msg;
    _test_id = 0;
#ifdef QUREPORTER
		_reporter = new QUREPORTER();
#else
		_reporter = new QUReporter();
#endif
    QUTestSuiteTracker::CurrentQUTestSuite(this);
  }
	~QUTestSuite() {
		delete _reporter;
	}
  int Add(QUTest *test) {
    _tests.push_back(test);
    return _test_id++;
  }  
  int RunAll(void) {
    unsigned fails = 0;
    unsigned passes = 0;
    int suite_start, suite_end;
    suite_start = clock();
		_reporter->StartingSuite(_suite_name);
    BeforeAll();
		_reporter->StartedSuite(_suite_name);
    for (std::list<QUTest *>::iterator iter = _tests.begin(); iter != _tests.end(); ++iter) {
      bool failed = false;
			_reporter->StartingTest(_suite_name, (*iter)->name_as_token());
      int test_start = clock();
      BeforeEach();
			_reporter->StartedTest(_suite_name, (*iter)->name_as_token());
      try {
        (*iter)->Run();
      } catch(std::string msg) {
        std::cout << msg << std::endl;
      } catch(...) {
        failed = true;
      }
			_reporter->StoppingTest(_suite_name, (*iter)->name_as_token());
      AfterEach();
      int test_end = clock();
      double diff = (test_end - test_start) / 1000.0;
      if (failed || (*iter)->fails()) {
        fails++;
				_reporter->FailedTest(_suite_name, (*iter)->name_as_token(), diff, (*iter)->fail_message());
      } else {
				passes++;
				_reporter->PassedTest(_suite_name, (*iter)->name_as_token(), diff);
			}
			_reporter->CompletedTest(_suite_name, (*iter)->name_as_token(), diff);
    }
		_reporter->StoppingSuite(_suite_name);
    AfterAll();
    suite_end = clock();
    double diff = (suite_end - suite_start) / 1000.0;
		_reporter->CompletedSuite(_suite_name, diff, passes, fails);
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
