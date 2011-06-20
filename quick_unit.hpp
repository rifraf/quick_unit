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
 *
 *  Plugin architecture for reporting allows you to replace
 *  the default output text formatter. Alternatively you can
 *  run additional reporters at the same time. See GitHub/readme.
 *
 *  The output stream can be changed from the default std::cout.
 *  See GitHub/readme.
 *
 *  Test Setup/Teardown methods can be declared. (Also per-suite
 *  setup/teardown.) See GitHub/readme.
 *
 *  Tests can use printf, or stream text to Output(). Such text
 *  gets routed through the reporters, so can be redirected to
 *  the stream that the reporters are using. See GitHub/readme.
 *
 * Tested on:
 *  Visual Studio 2010
 *  Visual Studio 2005
 *  MinGW GCC (5.16)
 *  Debian linux
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
#include <stdarg.h>
#undef assert

namespace quick_unit {
class QUTest;
class QUTestSuite;

/******************************************************************************/
class QUStdOutTracker {
/******************************************************************************/
public:
  // Used to track the current output stream.
  // When called with NULL argument it just returns the current output stream
  // When called with a new_stream, the output stream gets changed
  static std::ostream &Output(std::ostream *new_stream = NULL) {
    static std::ostream *current;
    if (!current) {
      current = &std::cout;
    }
    if (new_stream) {
      current = new_stream;
    }
    return *current;
  }
};
#define TEST_OUTPUT(stream) quick_unit::QUStdOutTracker::Output(&stream);

/******************************************************************************/
class QUReporter {  // Base class for all test reporters
/******************************************************************************/
protected:
  QUReporter *_chain;
  static std::ostream &Output(void) { return QUStdOutTracker::Output(); }

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
  virtual void TestOutput(const std::string &suite_name, const std::string &test_name, const std::string &text) {} // Before CompletedTest();
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
	void StartingSuite(const std::string &suite_name) {
    Output() << std::endl << "====================================================" << std::endl << "Starting " << suite_name << " at " << QUReporter::current_time() << std::endl;
	}
	void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    Output() << std::endl << "----------------------------------------------------" << std::endl << "Finished " << suite_name << " at " << QUReporter::current_time() <<
			"Passes: " << passes << " Fails: " << fails << std::endl << "----------------------------------------------------" << std::endl;
	}
	void StartingTest(const std::string &suite_name, const std::string &test_name) {
    Output() << "Test: " << test_name << " => ";
	}
	void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
		Output() << "FAILED. " << fail_message << std::endl;
	}
	void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {
		Output() << "OK." << std::endl;
	}
  void TestOutput(const std::string &suite_name, const std::string &test_name, const std::string &text) {
    Output()
      << "-- Output --" << std::endl
      << text
      << "------------" << std::endl;
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
  std::ostringstream _output;
  std::string _output_message;
private:
private:
  int format_arg_list(std::string& out, int length, const char *fmt, va_list args) {
    if (!fmt) return -1;
    int result = 0;
    char *buffer = NULL;
    buffer = new char [length + 1];
    memset(buffer, 0, length + 1);
		#ifdef _MSC_VER
    result = vsnprintf_s(buffer, length + 1, _TRUNCATE, fmt, args);
		#else
    result = vsnprintf(buffer, length, fmt, args);
		#endif
    out = buffer;
    delete [] buffer;
    return result;
  }

public:
  QUTest(const char *msg) {
    _fails = 0;
    _passes = 0;
    _assertions = 0;
    _fail_message = "";
    _test_name = msg;
  }
  virtual void Run(void) = 0;

  const std::string &name() { return _test_name; }
  
  std::ostream &Output() {return _output;}
  const std::string &test_output() {_output_message = _output.str(); _output.seekp(0, std::ios::beg); return _output_message;}

  int printf(const char* fmt, ...) {
    int count = 0;
    va_list args;
    va_start(args, fmt);
    std::string s;
    int length = 256;
    int result = format_arg_list(s, length, fmt, args);
    count += result;
    va_end(args);
    if (result >= 256) {
      va_start(args, fmt);
      format_arg_list(s, result + 1, fmt, args);
      va_end(args);
    }
    _output << s;
    return count;
  }

  int passes() { return _passes; }
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
  
  void assert(bool truth, const char *msg = NULL) {
    _assertions++;
    if (truth) {
      _info_message.str("");
      _passes++;
    } else {
      if (msg) {
        _fail_message = msg;
      } else {
        std::ostringstream os;
        os << "assertion #" << _assertions;
        _fail_message = os.str();
      }
      _fails++;
      throw new QUTestFail();
    }
  }

  template <class T>
  void assert_equal(const T& a, const T& b, const char *msg = NULL) {
    _info_message << " (Expected: " << a << ", got: " << b << ")";
    assert(a == b, msg);
  }

  template <class T>
  void assert_not_equal(const T& a, const T& b, const char *msg = NULL) {
    _info_message << " (Expected difference. Both: " << a << ")";
    assert(a != b, msg);
  }

  void assert_equal(const char *a, const char *b, const char *msg = NULL) {
    _info_message << " (Expected: " << a << ", got: " << b << ")";
    assert(strcmp(a,b) == 0, msg);
  }

  void assert_not_equal(const char *a, const char *b, const char *msg = NULL) {
    _info_message << " (Expected difference. Both: " << a << ")";
    assert(strcmp(a,b) != 0, msg);
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
    unsigned total_fails = 0;
    if (_chain) {
      total_fails += _chain->RunAll();
    }
    std::list<QUReporter *> reporters;
    QUReporter *r = _reporter;
    while(r) {
      reporters.push_back(r);
      r = r->chain();
    }
    unsigned passes = 0;
    unsigned fails = 0;
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
        total_fails++;
				EACH_QUREPORTER_REVERSE(FailedTest(_suite_name, test_name, duration, (*iter)->fail_message()))
      } else {
				passes++;
				EACH_QUREPORTER_REVERSE(PassedTest(_suite_name, test_name, duration))
			}
      std::string output = (*iter)->test_output();
      if (!output.empty()) {
  			EACH_QUREPORTER_REVERSE(TestOutput(_suite_name, test_name, output))
      }
			EACH_QUREPORTER_REVERSE(CompletedTest(_suite_name, test_name, duration))
    }
		EACH_QUREPORTER_REVERSE(StoppingSuite(_suite_name))
    AfterAllTests();
		EACH_QUREPORTER_REVERSE(CompletedSuite(_suite_name, (clock() - suite_start) / 1000.0, passes, fails))
    return total_fails;
  }
};

/******************************************************************************/
/* Macros for creating a TEST */
#define _UNIQ_ID_2(x,y) x##y
#define _UNIQ_ID_1(x,y) _UNIQ_ID_2(x,y)
#define _UNIQ_ID_(x) _UNIQ_ID_1(x,  __LINE__ )

// MUST be on a single line
#define TEST(name) namespace { class _UNIQ_ID_(QUTest) : public QUTest {public: _UNIQ_ID_(QUTest)() : QUTest(#name) {if (QUTestSuiteTracker::CurrentQUTestSuite()) {QUTestSuiteTracker::CurrentQUTestSuite()->Add(this);}} void Run(void); } static _UNIQ_ID_(test);} void _UNIQ_ID_(QUTest)::Run(void)

/******************************************************************************/
/* Macros for creating a SHOULD message */
#define _LINE_PASTE2(x,y) x#y
#define _LINE_PASTE(x,y) _LINE_PASTE2(x,y)
#define SHOULD(msg) _LINE_PASTE("line ",__LINE__) ": Should " # msg

/******************************************************************************/
/* Macros for creating a SUITE */
#define BEGIN_SUITE(name) \
using namespace quick_unit; namespace {\
class _UNIQ_ID_(QUSuite) : public QUTestSuite{ public: _UNIQ_ID_(QUSuite)() : QUTestSuite(#name) {}
#define END_SUITE_AS(name) } static name; }
#define END_SUITE } static _UNIQ_ID_(QUSuite); }
#define DECLARE_SUITE(name) BEGIN_SUITE(name) END_SUITE

#define SETUP_SUITE void BeforeAllTests()
#define TEARDOWN_SUITE void AfterAllTests()
#define SETUP void BeforeEachTest()
#define TEARDOWN void AfterEachTest()

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
