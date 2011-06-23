/* 
 * quick_unit.hpp : http://github.com/rifraf/quick_unit
 * Author: David Lake
 *
 * Description:
 *  This add-in to quick_unit provides support for the 'require' syntax
 *  shown below.
 *
 * TEST(Experiments: require syntax) {
 *
 *   REQUIRE(First Experiment) >> true;

 *   REQUIRE(Second Experiment) >>
 *     1 >>
 *     !0 >>
 *     is_true(1) >>
 *     is_false(0) >>
 *     equal(1, 2-1) >>
 *     not_equal(1,2) >>
 *     equal("xy", "x""y") >>
 *     not_equal("xy", "x""z")
 *   ;
 * }
 *
 * All of the >> elements are requiresd to be true for the test to pass.
 * The only slight quirk to watch for is that the assertions get checked
 * in reverse order (right to left).
 * 
 */

#ifndef QUICK_UNIT_REQ_SYNTAX_HPP
#define	QUICK_UNIT_REQ_SYNTAX_HPP

namespace quick_unit {

// The Requirer provides support for using the >> syntax in tests
class Requirer {
  
protected:
  int _line;
  std::string _msg;
  QUTest *_test;

public:
  Requirer &require(QUTest *test, int line, const char *msg) {
    _line = line;
    _msg = msg;
    _test = test;
    return *this;
  }
  const std::string &message(void) {return _msg;}

  const char *fail_message(void) {
    std::ostringstream os;
    os << "line " << _line << ": " << _msg;
    _msg = os.str();
    return _msg.c_str();
  }

  const char *fail_message(const std::string &msg) {
    std::ostringstream os;
    os << "line " << _line << ": " << _msg << " " << msg;
    _msg = os.str();
    return _msg.c_str();
  }

  Requirer& test_truth(bool truth) {
		Qu_Result result = {truth, ""};
    _test->_assert(result, truth ? "" : fail_message());
    return *this;
  }

  Requirer& test_truth(bool truth, const std::string &msg) {
		Qu_Result result = {truth, ""};
    _test->_assert(result, truth ? "" : fail_message(msg));
    return *this;
  }

};

// QUTestRequirer extends a quick_unit test to embed a reference to the current requirer.
class QUTestRequirer : public quick_unit::QU_TEST_ANCESTOR {
  public:
    QUTestRequirer(const char *msg) : quick_unit::QU_TEST_ANCESTOR(msg) {}
    quick_unit::Requirer current_require;
};
#undef QU_TEST_ANCESTOR
#define QU_TEST_ANCESTOR QUTestRequirer

} /* quick_unit */

quick_unit::Requirer& operator>>(quick_unit::Requirer &requirer, bool truth) {
  return requirer.test_truth(truth);
}
quick_unit::Requirer& operator>>(quick_unit::Requirer &requirer, quick_unit::Qu_Result result) {
  return requirer.test_truth(result.pass, result.msg);
}

#define REQUIRE(msg) current_require.require(this, __LINE__, "Require '" # msg "'")

#endif	/* QUICK_UNIT_REQ_SYNTAX_HPP */

