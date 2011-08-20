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
 *    assert(3 == 1 + 2, SHOULD(add numbers));
 *  }
 *
 *  TEST("the compiler can subtract") {
 *    assert_equal(1 , 2 - 1, SHOULD(subtract numbers));
 *  }
 *
 *  int main(int argc, char *argv[]) {
 *    return RUN_TESTS();
 *  }
 *
 * Inspired by Ruby. Thanks Matz. Thanks Dave Thomas.
 *
 * Advanced features:
 *
 *  If you don't like the use of 'assert' because you might confuse
 *  it with the macro version in assert.h, then simply define QU_ASSERT
 *  as your your_word of choice. e.g. #define QU_ASSERT verify.
 *  If you prefer a different word to SHOULD, just define it to map on to
 *  QU_SHOULD. e.g. #define CAN QU_SHOULD.
 *
 *  A plugin architecture for reporting allows you to replace
 *  the default output text formatter. You can also chain reporters
 *  together to run more than one at the same time. See GitHub/readme.
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
#define QUICK_UNIT_HPP

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

namespace quick_unit {

struct Qu_Result {
  bool pass;
  std::string msg;
  Qu_Result(bool truth) : pass(truth), msg("") {}
  Qu_Result(bool truth, const std::string expectation) {
    pass = truth;
    msg = expectation;
  }
};

class QUTestSuite;

/******************************************************************************/
// Construction tools
#define QU_TOKENIZE(m, n) m##n
#define QU_TOKEN_MERGE(m, n) QU_TOKENIZE(m, n)
#define QU_STRINGIZE2(x,y) x#y
#define QU_STRINGIZE(x,y) QU_STRINGIZE2(x,y)

#define QU_UNIQ_ID(x)  QU_TOKEN_MERGE(x,  __LINE__)

#ifndef QU_ASSERT
 #undef assert
 #define QU_ASSERT assert
#endif

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
  virtual void StoppingSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {} // Before AfterAllTests();
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
  void StoppingSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
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

#define ADD_MATCHER(name,...) Qu_Result name(__VA_ARGS__)
#define MATCHER(condition, ...) \
    bool is_true = condition;\
    if (!is_true) {\
      _expectation_builder.str("");\
      _expectation_builder << __VA_ARGS__;\
      _expectation = _expectation_builder.str();\
    }\
    return result(is_true, _expectation);
#define ADD_ASSERTION(name,...) void QU_TOKEN_MERGE(QU_ASSERT,_ ## name)(__VA_ARGS__, const char *msg = NULL)
#define ASSERTION(test) _assert(test, msg);

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
  std::ostringstream _expectation_builder;
  std::string _full_message;
  std::ostringstream _output;
  std::string _output_message;
  std::string _expectation;

protected:
  // test printf helper
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
    _test_name = msg;
     Reset();
  }
  void Reset() {
    _fails = 0;
    _passes = 0;
    _assertions = 0;
    _fail_message = "";
     _info_message.str("");
  }
  virtual void Run(void) = 0; // Must be subclassed
  const std::string &test_name() { return _test_name; }

  // Pass/fail tracking
  int passes() { return _passes; }
  int fails() {
    if (_passes + _fails == 0) {
      return 1; // No asserts - not a valid test. Report as a fail
    }
    return _fails;
  }
  const std::string &fail_message() {
    if (_passes + _fails == 0) {
      _full_message = "No assertions were executed/completed";
    } else {
      _full_message = _fail_message + _info_message.str();
    }
    return _full_message;
  }

  void force_fail_message(const char *forced_message) {
    std::ostringstream os;
    os << forced_message << " Completed assertions:" << _assertions;
    _fail_message = os.str();
    _fails++;
  }

  // Test output text helpers
  const std::string &test_output_text() {_output_message = _output.str(); _output.seekp(0, std::ios::beg); return _output_message;}
  std::ostream &Output() {return _output;}
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

  Qu_Result result(bool truth, const std::string expectation) {
    Qu_Result X(truth, expectation);
    return X;
  }

  // Result matcher: truth
  ADD_MATCHER(is_true, bool truth) {
    MATCHER(truth, " (Expected result was not true)");
  }
  // Result matcher: falsity
  ADD_MATCHER(is_false, bool truth) {
    MATCHER(!truth, " (Expected result was not false)");
  }
  // Result matchers: Equality
  template <class T> ADD_MATCHER(equal, const T& a, const T& b) {
    MATCHER((a == b), " (Expected: " << a << ", got: " << b << ")");
  }
  ADD_MATCHER(equal, const char *a, const char *b) {
    MATCHER((strcmp(a,b) == 0), " (Expected: " << a << ", got: " << b << ")");
  }
  // Result matchers: Inequality
  template <class T> ADD_MATCHER(not_equal, const T& a, const T& b) {
    MATCHER((a != b), " (Expected difference. Both: " << a << ")");
  }
  ADD_MATCHER(not_equal, const char *a, const char *b) {
    MATCHER((strcmp(a,b) != 0), " (Expected difference. Both: " << a << ")");
  }
  // Result matcher: inclusion/exclusion
  ADD_MATCHER(includes, const char *inclusion, const char *text) {
    MATCHER((strstr(text, inclusion) != NULL), " (Expected to see '" << inclusion << "' in '"<< text << "')");
  }
  ADD_MATCHER(excludes, const char *inclusion, const char *text) {
    MATCHER((strstr(text, inclusion) == NULL), " (Expected not to see '" << inclusion << "' in '"<< text << "')");
  }

  // The core assertion handler
  void _assert(Qu_Result result, const char *msg = NULL) {
    _assertions++;
    if (result.pass) {
      _info_message.str("");
      _passes++;
    } else {
      _info_message << result.msg;
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

  // Assertions
  // ... true/false
  void QU_ASSERT(bool truth, const char *msg = NULL) { _assert(is_true(truth), msg); }
  ADD_ASSERTION(true, bool truth)  {ASSERTION(is_true(truth)); }
  ADD_ASSERTION(false, bool truth) {ASSERTION(is_false(truth)); }
  // ... equal
  template <class T> ADD_ASSERTION(equal, const T& a, const T& b) {ASSERTION(equal(a,b)); }
  ADD_ASSERTION(equal, const char *a, const char *b) {ASSERTION(equal(a,b));}
  // ... not equal
  template <class T> ADD_ASSERTION(not_equal, const T& a, const T& b) {ASSERTION(not_equal(a,b)); }
  ADD_ASSERTION(not_equal, const char *a, const char *b) {ASSERTION(not_equal(a,b));}
  // Assertion: inclusion/exclusion
  ADD_ASSERTION(include, const char *inclusion, const char *text) {ASSERTION(includes(inclusion, text));}
  ADD_ASSERTION(exclude, const char *inclusion, const char *text) {ASSERTION(excludes(inclusion, text));}
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
      std::string test_name = (*iter)->test_name();
      EACH_QUREPORTER(StartingTest(_suite_name, test_name))
      int test_start = clock();
      BeforeEachTest();
      EACH_QUREPORTER(StartedTest(_suite_name, test_name))
      try {
        (*iter)->Reset();
        (*iter)->Run();
      } catch(QUTestFail * /*err*/) {
        // Failed assertions cause us to come here
        failed = true;
      } catch(...) {
        failed = true;
        (*iter)->force_fail_message("unexpected exception in the test");
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
      std::string output = (*iter)->test_output_text();
      if (!output.empty()) {
        EACH_QUREPORTER_REVERSE(TestOutput(_suite_name, test_name, output))
      }
      EACH_QUREPORTER_REVERSE(CompletedTest(_suite_name, test_name, duration))
    }
    EACH_QUREPORTER_REVERSE(StoppingSuite(_suite_name, (clock() - suite_start) / 1000.0, passes, fails))
    AfterAllTests();
    EACH_QUREPORTER_REVERSE(CompletedSuite(_suite_name, (clock() - suite_start) / 1000.0, passes, fails))
    return total_fails;
  }
};

/******************************************************************************/
/* Macros for creating a TEST */
#define QU_TEST_ANCESTOR QUTest
#define EXTEND_TEST(name) class name : public quick_unit::QU_TEST_ANCESTOR { public: name(const char *msg) : quick_unit::QU_TEST_ANCESTOR(msg) {}
#define END_EXTEND_TEST };

// MUST be on a single line
#define TEST(name) namespace { class QU_UNIQ_ID(QUTest) : public QU_TEST_ANCESTOR {public: QU_UNIQ_ID(QUTest)() : QU_TEST_ANCESTOR(#name) {if (QUTestSuiteTracker::CurrentQUTestSuite()) {QUTestSuiteTracker::CurrentQUTestSuite()->Add(this);}} void Run(void); } static QU_UNIQ_ID(test);} void QU_UNIQ_ID(QUTest)::Run(void)

/******************************************************************************/
/* Macros for creating a SHOULD message */
#define QU_SHOULD(msg) QU_STRINGIZE("line ",__LINE__) ": Should "  # msg "."
#define SHOULD QU_SHOULD

/******************************************************************************/
/* Macros for creating a SUITE */
#define BEGIN_SUITE(name) \
using namespace quick_unit; namespace {\
class QU_UNIQ_ID(QUSuite) : public QUTestSuite{ public: QU_UNIQ_ID(QUSuite)() : QUTestSuite(#name) {}
#define END_SUITE_AS(name) } static name; }
#define END_SUITE } static QU_UNIQ_ID(QUSuite); }
#define DECLARE_SUITE(name) BEGIN_SUITE(name) END_SUITE

#define SETUP_SUITE void BeforeAllTests()
#define TEARDOWN_SUITE void AfterAllTests()
#define SETUP void BeforeEachTest()
#define TEARDOWN void AfterEachTest()

#define RUN_TESTS() QUTestSuiteTracker::CurrentQUTestSuite()->RunAll()
/******************************************************************************/
/* Macros for REPORTERs */
#define TEST_REPORTER(name) \
using namespace quick_unit; namespace  { class QU_UNIQ_ID(QUReporter) : public name ##Reporter { public: QU_UNIQ_ID(QUReporter)() { QUTestSuiteTracker::CurrentQUReporter(this);} } static QU_UNIQ_ID(Reporter); }

#define ADDITIONAL_REPORTER(name) \
using namespace quick_unit; namespace  { class QU_UNIQ_ID(QUReporter) : public name ##Reporter { public: QU_UNIQ_ID(QUReporter)() { QUTestSuiteTracker::CurrentQUReporter(this, true);} } static QU_UNIQ_ID(Reporter); }

#define BEGIN_REPORTER(name) namespace quick_unit { class name ##Reporter : public QUReporter { public:
#define END_REPORTER() }; }

} /* namespace */
TEST_REPORTER(Default)

#endif  /* QUICK_UNIT_HPP */
