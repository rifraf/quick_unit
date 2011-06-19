/*
 * quick_unit_netbeans.hpp : http://github.com/rifraf/quick_unit
 * Author: David Lake
 * Description:
 *   This is a custom reporter add-in for quick_unit. It emits messages that
 *   are compatible with the Netbeans Simple C++ Test output format, so you can
 *   use quick_unit to implement tests that integrate with Netbeans projects.
 *   For a real example, see quick_unit/MinGW_Netbeans/VCLTests.cpp in the github
 *   repository.
 * 
 * Quick example:
 *   #include "quick_unit.hpp"
 *   #include "quick_unit_netbeans.hpp"
 *
 *   TEST_REPORTER(Netbeans)
 *   DECLARE_SUITE(MyFirstTests)
 *   TEST("the compiler can add") {
 *     assert(3 == 1 + 2, SHOULD(add numbers));
 *   }
 */

#ifndef QUICK_UNIT_NETBEANS_HPP
#define	QUICK_UNIT_NETBEANS_HPP

BEGIN_REPORTER(Netbeans)
  void StartingSuite(const std::string &suite_name) {
    Output() << "%SUITE_STARTING% " << suite_name << std::endl;
  }
  void StartedSuite(const std::string &suite_name)  {
    Output() << "%SUITE_STARTED%" << std::endl;
  }
  void CompletedSuite(const std::string &suite_name, double duration, unsigned passes, unsigned fails) {
    Output() << std::endl << "%SUITE_FINISHED% time=" << std::fixed << duration << std::endl;
  }
  void StartingTest(const std::string &suite_name, const std::string &test_name) {
    Output() << "%TEST_STARTED% " << without_whitespace(test_name) << " (" << suite_name << ")" << std::endl;
  }
  void FailedTest(const std::string &suite_name, const std::string &test_name, double duration, const std::string &fail_message) {
    Output() << "%TEST_FAILED% time=" << std::fixed << duration << " testname=" << without_whitespace(test_name) << " (" << suite_name << ") message=" << fail_message << std::endl;
  }
  void CompletedTest(const std::string &suite_name, const std::string &test_name, double duration) {
    Output() << "%TEST_FINISHED% time=" << std::fixed << duration << " " << without_whitespace(test_name) << " (" << suite_name << ")" << std::endl;
  }

private:
  // Netbeans doesn't like spaces in status lines
  std::string without_whitespace(const std::string &str) {
    std::string rep(str);
    std::replace( rep.begin(), rep.end(), ' ', '_' );
    return rep;
  }
END_REPORTER()

#endif /* QUICK_UNIT_NETBEANS_HPP */
