#pragma once

#include <DynamicFlow/Error.h>

#include <sstream>
#include <mutex>

namespace flw::test {
class ErrorTest : public std::runtime_error {
public:
  using std::runtime_error::runtime_error; 
};

template <std::size_t ExpectedIncrement, typename Subject>
class IncrementGuard {
public:
  IncrementGuard(Subject &subject)
      : to_check(subject), initial_size(subject.size()){};

  ~IncrementGuard() {
    EXPECT_EQ(to_check.size(), initial_size + ExpectedIncrement);
  }

private:
  Subject &to_check;
  const std::size_t initial_size;
};

template <typename Subject> using UnchangedGuard = IncrementGuard<0, Subject>;

template<bool Throw>
struct ComposerLambda {
  ComposerLambda() = default;

  template<typename ... ARGS>
  std::string operator()(const ARGS& ... args) const {
    return detail::merge<0>(args...);
  }
};

template<>
struct ComposerLambda<true> {
  ComposerLambda() = default;

  template<typename ... ARGS>
  std::string operator()(const ARGS& ... args) const {
    if(flag_) {
      flag_ = false;
      throw ErrorTest{"foo"};
    }
    return detail::merge<0>(args...);
  }

private:
  mutable bool flag_{true};
};
}
