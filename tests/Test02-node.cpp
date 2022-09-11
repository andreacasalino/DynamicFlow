/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include "ValueExtractor.h"
#include <DynamicFlow/Node.hpp>

namespace {
template <typename T, typename... Deps>
class TestBind : public ::testing::Test, public flw::Node<T, Deps...> {
public:
  TestBind() : flw::Node<T, Deps...>([](const Deps &...) { return T{}; }) {}
};
} // namespace

namespace {
using TestBind_int__int = TestBind<int, int>;
}
TEST_F(TestBind_int__int, bind) {
  flw::ValueTypedWithErrors<int> dep_0;

  this->bind<0>(dep_0);
  const auto *expected_ptr = this->flw::detail::Dependency<0, int>::dependency_;
  EXPECT_EQ(&dep_0, expected_ptr);
}

namespace {
using TestBind_int__int_string = TestBind<int, int, std::string>;
}
TEST_F(TestBind_int__int_string, bind) {
  flw::ValueTypedWithErrors<int> dep_0;
  this->bind<0>(dep_0);

  flw::ValueTypedWithErrors<std::string> dep_1;
  this->bind<1>(dep_1);

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<0, int>::dependency_;
    EXPECT_EQ(&dep_0, expected_ptr);
  }

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<1, std::string>::dependency_;
    EXPECT_EQ(&dep_1, expected_ptr);
  }
}

namespace {
using TestBind_int__int_string_int_string =
    TestBind<int, int, std::string, int, std::string>;
}
TEST_F(TestBind_int__int_string_int_string, bind) {
  flw::ValueTypedWithErrors<int> dep_0;
  this->bind<0>(dep_0);

  flw::ValueTypedWithErrors<std::string> dep_1;
  this->bind<1>(dep_1);

  flw::ValueTypedWithErrors<int> dep_2;
  this->bind<2>(dep_2);

  flw::ValueTypedWithErrors<std::string> dep_3;
  this->bind<3>(dep_3);

  flw::Values expected_deps_set;

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<0, int>::dependency_;
    EXPECT_EQ(&dep_0, expected_ptr);
    expected_deps_set.emplace(expected_ptr);
  }

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<1, std::string>::dependency_;
    EXPECT_EQ(&dep_1, expected_ptr);
    expected_deps_set.emplace(expected_ptr);
  }

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<2, int>::dependency_;
    EXPECT_EQ(&dep_2, expected_ptr);
    expected_deps_set.emplace(expected_ptr);
  }

  {
    const auto *expected_ptr =
        this->flw::detail::Dependency<3, std::string>::dependency_;
    EXPECT_EQ(&dep_3, expected_ptr);
    expected_deps_set.emplace(expected_ptr);
  }

  EXPECT_EQ(expected_deps_set, this->dependencies());
}

#include <sstream>

namespace {
class ErrorTest : public flw::Error {
public:
  ErrorTest() : flw::Error{""} {};
};

std::string evaluation_test(const int &i, const std::string &s) {
  std::stringstream stream;
  if (0 == i) {
    throw ErrorTest{};
  }
  stream << i << '-' << s;
  return stream.str();
}

class TestUpdate : public ::testing::Test,
                   public flw::Node<std::string, int, std::string> {
public:
  TestUpdate()
      : flw::Node<std::string, int, std::string>(
            evaluation_test,
            std::make_unique<
                flw::ValueTypedWithErrors<std::string, ErrorTest>>()) {}

  void SetUp() override {
    this->bind<0>(s0);
    this->bind<1>(s1);
  }

protected:
  flw::ValueTypedWithErrors<int> s0;
  flw::ValueTypedWithErrors<std::string> s1;
};
} // namespace

TEST_F(TestUpdate, update_not_ready) {
  EXPECT_EQ(this->status(), flw::NodeStatus::UPDATE_NOT_POSSIBLE);
  EXPECT_THROW(this->update(), flw::Error);
  EXPECT_EQ(this->getValue().status(), flw::ValueStatus::UNSET);
}

TEST_F(TestUpdate, update_possible_leading_to_value) {
  s0.update([]() { return 1; });
  s1.update([]() { return "hello"; });
  EXPECT_EQ(this->status(), flw::NodeStatus::UPDATE_POSSIBLE);
  this->update();
  EXPECT_EQ(this->status(), flw::NodeStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(this->getValue().status(), flw::ValueStatus::VALUE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(this->getValue()),
            evaluation_test(flw::ValueExtractor::impl().get(s0),
                            flw::ValueExtractor::impl().get(s1)));
}

TEST_F(TestUpdate, update_possible_leading_to_exception) {
  s0.update([]() { return 0; });
  s1.update([]() { return "hello"; });
  EXPECT_EQ(this->status(), flw::NodeStatus::UPDATE_POSSIBLE);
  this->update();
  EXPECT_EQ(this->status(), flw::NodeStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(this->getValue().status(), flw::ValueStatus::EXCEPTION);
  EXPECT_THROW(this->getValue().reThrow(), ErrorTest);
}
