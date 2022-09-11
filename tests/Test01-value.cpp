/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include "CheckCPlusPlus20.h"
#include "ValueExtractor.h"
#include <DynamicFlow/Value.hpp>

#include <sstream>

namespace {
static constexpr int INT_VALUE = 55000;
int make_value() { return INT_VALUE; }

class ErrorTest : public flw::Error {
public:
  ErrorTest() : flw::Error{""} {}
};

int make_exception() {
  throw ErrorTest{};
  return 0;
}
} // namespace

TEST(Value, statusCheck) {
  {
    flw::ValueTypedWithErrors<int> val;
    EXPECT_EQ(val.status(), flw::ValueStatus::UNSET);
  }

  {
    flw::ValueTypedWithErrors<int> val;
    EXPECT_EQ(val.epoch(), 0);
    val.update(make_value);
    EXPECT_EQ(val.epoch(), 1);
    EXPECT_EQ(val.status(), flw::ValueStatus::VALUE);
    EXPECT_EQ(flw::ValueExtractor::impl().get(val), INT_VALUE);

    val.update([]() { return INT_VALUE + 1; });
    EXPECT_EQ(val.epoch(), 2);
    EXPECT_EQ(val.status(), flw::ValueStatus::VALUE);
    EXPECT_EQ(flw::ValueExtractor::impl().get(val), INT_VALUE + 1);

    val.reset();
    EXPECT_EQ(val.status(), flw::ValueStatus::UNSET);
  }

  {
    flw::ValueTypedWithErrors<int, ErrorTest> val;
    EXPECT_EQ(val.epoch(), 0);
    val.update(make_exception);
    EXPECT_EQ(val.epoch(), 1);
    EXPECT_EQ(val.status(), flw::ValueStatus::EXCEPTION);
    EXPECT_THROW(flw::ValueExtractor::impl().get(val), flw::UnsetValueError);
    EXPECT_THROW(val.reThrow(), ErrorTest);

    val.reset();
    EXPECT_EQ(val.status(), flw::ValueStatus::UNSET);
  }
}

namespace {
class WasUpdatedGuard {
public:
  WasUpdatedGuard(const flw::Value &subject)
      : subject_(subject), initial_epoch(subject.epoch()) {}

  ~WasUpdatedGuard() { EXPECT_EQ(subject_.epoch(), initial_epoch + 1); }

private:
  const flw::Value &subject_;
  const std::size_t initial_epoch;
};

class WasNotUpdatedGuard {
public:
  WasNotUpdatedGuard(const flw::Value &subject)
      : subject_(subject), initial_epoch(subject.epoch()) {}

  ~WasNotUpdatedGuard() { EXPECT_EQ(subject_.epoch(), initial_epoch); }

private:
  const flw::Value &subject_;
  const std::size_t initial_epoch;
};
} // namespace

TEST(Value, LazyUpdate) {
  CHECK_CPLUSPLUS_20

  {
    flw::ValueTypedWithErrors<int> val;
    const int num = 5;
    val.update([&num]() { return num; });
    WasNotUpdatedGuard guard(val);
    val.update([&num]() { return num; });
  }

  {
    class UnComparable {
    public:
      UnComparable() = default;
      UnComparable(UnComparable &&) = default;
    };

    flw::ValueTypedWithErrors<UnComparable> val;
    val.update([]() { return UnComparable{}; });
    WasUpdatedGuard guard(val);
    val.update([]() { return UnComparable{}; });
  }
}
