/* 
 * File:   quick_unit_reqtrace.hpp
 * Author: David
 *
 * Created on 20 June 2011, 19:48
 */

#ifndef QUICK_UNIT_REQTRACE_HPP
#define	QUICK_UNIT_REQTRACE_HPP

namespace quick_rreqs {
  /******************************************************************************/
  class QUReqTracker {
  public:
    static std::list<std::string> &Reqs(const char *req = NULL) {
      static std::list<std::string> reqlist;
      if (req) {
        reqlist.push_back(req);
      } else {
        reqlist.clear();
      }
      return reqlist;
    }
  };
}

/******************************************************************************/
BEGIN_REPORTER(ReqTrace)
  unsigned _test_count;

  void StartingSuite(const std::string &suite_name) {
    _test_count = 0;
    Output()
      << "........................................" << std::endl
      << suite_name << std::endl
      << "Test at " << current_time() << std::endl
      << "........................................" << std::endl << std::endl;
  }

  void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    Output()
      << "........................................" << std::endl
      << _test_count << " tests with " << passes + fails << " asserts in " << duration << "s" << std::endl
      << " Passes: " << passes << ", Failures: " << fails << std::endl
      << "RESULT=" << (fails ? "FAIL" : "PASS") << std::endl
      << "........................................" << std::endl << std::endl;
  }
  
  void StartingTest(const std::string &suite_name, const std::string &test_name) {
    (void)quick_rreqs::QUReqTracker::Reqs(NULL);
    _test_count++;
	}

  void TestOutput(const std::string &suite_name, const std::string &test_name, const std::string &text) {
    Output() << text << std::endl;
  }
  
	void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
		Output() << "--------- FAIL -------------------" << std::endl;
    Output() << test_name << "(" << suite_name << ")" << std::endl;
    Output() << fail_message << std::endl << std::endl;
    
    OutputValidateMessages("Fail", suite_name, test_name);
	}
	void PassedTest(const std::string &suite_name, const std::string &test_name, double duration) {
		Output() << "--------- PASS -------------------" << std::endl;
    Output() << test_name << "(" << suite_name << ")" << std::endl << std::endl;
    
    OutputValidateMessages("Pass", suite_name, test_name);
	}

  private:
    void OutputValidateMessages(const char *kind, const std::string &suite_name, const std::string &test_name) {
      std::list<std::string> reqs = quick_rreqs::QUReqTracker::Reqs("");
      for (std::list<std::string>::iterator iter = reqs.begin(); iter != reqs.end(); ++iter) {
        std::string req = *iter;
        if (!req.empty()) {
          Output() << "[$Validates " << req << " : " << kind << " " << test_name << "(" << suite_name << ")]"  << std::endl;
        }
      }
    }

  // Helper: returns the current date/time
	static const char *current_time(void) {
    time_t rawtime;
    struct tm * timeinfo;
    static char buffer[80];
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
		#ifdef _MSC_VER
		#else
		#endif
    strftime(buffer,80,"%Y-%m-%dT%H:%M:%S%Z",timeinfo);
    return buffer;

	}
END_REPORTER()

class QUTestReqs : public quick_unit::QU_TEST_ANCESTOR {
  public:
    QUTestReqs(const char *msg) : quick_unit::QU_TEST_ANCESTOR(msg) {}
    void _Tests(const char *first_req, ...) {
      const char *current_req = first_req;
      va_list param_list;
      va_start(param_list, first_req);
      while (current_req) {
        (void)quick_rreqs::QUReqTracker::Reqs(current_req);
        current_req = va_arg(param_list, const char *);
      }
      va_end(param_list);
    }
    quick_unit::Requirer requirer;
};
#define TESTS(...) _Tests( __VA_ARGS__, NULL)

#undef QU_TEST_ANCESTOR
#define QU_TEST_ANCESTOR QUTestReqs

#endif	/* QUICK_UNIT_REQTRACE_HPP */

