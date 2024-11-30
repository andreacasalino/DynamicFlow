/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/Value.hxx>

TEST(ValueSourceTest, ctor) {
  flw::ValueSource<std::string> src{"hello world"};

  const auto *ptr = src.getValue();
  ASSERT_TRUE(ptr);
  EXPECT_EQ(*ptr, "hello world");
}

TEST(ValueSourceTest, update) {
  flw::ValueSource<std::string> src{"hello world"};
  src.update("another hello");

  const auto *ptr = src.getValue();
  ASSERT_TRUE(ptr);
  EXPECT_EQ(*ptr, "another hello");
}

struct ErrorA : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct ErrorB : std::runtime_error {
  using std::runtime_error::runtime_error;
};

namespace std {
bool operator==(const std::exception &a, const std::exception &b) {
  return a.what() == b.what();
}
} // namespace std

struct ValueCallBacksTest : ::testing::Test,
                            flw::ValueCallBacks<std::string, ErrorA, ErrorB> {
  std::tuple<std::vector<std::string>, std::vector<ErrorA>, std::vector<ErrorB>,
             std::vector<std::runtime_error>>
      values;

  void clear() {
    std::get<0>(values).clear();
    std::get<1>(values).clear();
    std::get<2>(values).clear();
    std::get<3>(values).clear();
  }

  bool empty() const {
    return std::get<0>(values).empty() && std::get<1>(values).empty() &&
           std::get<2>(values).empty() && std::get<3>(values).empty();
  }

  template <typename T> void addCB() {
    if constexpr (std::is_same_v<T, std::string>) {
      this->addOnValue([this](const std::string &str) {
        std::get<std::vector<std::string>>(values).emplace_back(str);
      });
    } else if constexpr (std::is_same_v<T, std::runtime_error>) {
      this->addOnError<std::exception>([this](const std::exception &e) {
        std::get<std::vector<std::runtime_error>>(values).emplace_back(
            e.what());
      });
    } else {
      this->addOnError<T>([this](const T &e) {
        std::get<std::vector<T>>(values).emplace_back(e.what());
      });
    }
  }

  template <typename ErrorT> std::vector<std::string> errorMessages() const {
    std::vector<std::string> res;
    for (const auto &e : std::get<std::vector<ErrorT>>(values)) {
      res.emplace_back(e.what());
    }
    return res;
  }
};

TEST_F(ValueCallBacksTest, empty) {
  this->onValue("");
  ASSERT_TRUE(this->empty());

  this->onError<ErrorA>(ErrorA{""});
  ASSERT_TRUE(this->empty());

  this->onError<ErrorB>(ErrorB{""});
  ASSERT_TRUE(this->empty());

  this->onError<std::exception>(std::runtime_error{""});
  ASSERT_TRUE(this->empty());
}

TEST_F(ValueCallBacksTest, onValueCB_onValue) {
  this->addCB<std::string>();

  this->onValue("foo");
  ASSERT_TRUE(std::get<std::vector<ErrorA>>(values).empty());
  ASSERT_TRUE(std::get<std::vector<ErrorB>>(values).empty());
  ASSERT_TRUE(std::get<std::vector<std::runtime_error>>(values).empty());

  ASSERT_EQ(std::get<std::vector<std::string>>(values),
            std::vector<std::string>{"foo"});
}

TEST_F(ValueCallBacksTest, onValueCB_onError) {
  this->addCB<std::string>();

  this->onError<ErrorA>(ErrorA{""});
  ASSERT_TRUE(this->empty());

  this->clear();
  this->onError<std::exception>(std::runtime_error{""});
  ASSERT_TRUE(this->empty());
}

TEST_F(ValueCallBacksTest, onErrorCB_onValue) {
  this->addCB<ErrorA>();

  this->onValue("foo");
  ASSERT_TRUE(this->empty());
}

TEST_F(ValueCallBacksTest, onErrorCB_onError) {
  this->addCB<ErrorA>();

  this->onError<ErrorA>(ErrorA{"foo"});
  ASSERT_TRUE(std::get<std::vector<std::string>>(values).empty());
  ASSERT_TRUE(std::get<std::vector<ErrorB>>(values).empty());
  ASSERT_TRUE(std::get<std::vector<std::runtime_error>>(values).empty());
  ASSERT_EQ(errorMessages<ErrorA>(), std::vector<std::string>{"foo"});

  this->clear();
  this->onError<ErrorB>(ErrorB{""});
  ASSERT_TRUE(this->empty());

  this->clear();
  this->onError<std::exception>(std::runtime_error{""});
  ASSERT_TRUE(this->empty());
}

template <typename TestedT> struct ValueAllCallBacksTest : ValueCallBacksTest {
  void SetUp() override {
    this->addCB<std::string>();
    this->addCB<ErrorA>();
    this->addCB<ErrorB>();
    this->addCB<std::runtime_error>();
  }
};

using ValueAllCallBacksTypes =
    ::testing::Types<std::string, ErrorA, ErrorB, std::runtime_error>;
TYPED_TEST_CASE(ValueAllCallBacksTest, ValueAllCallBacksTypes);

TYPED_TEST(ValueAllCallBacksTest, check) {
  if constexpr (std::is_same_v<TypeParam, std::string>) {
    this->onValue("foo");
  } else if constexpr (std::is_same_v<TypeParam, std::runtime_error>) {
    this->template onError<std::exception>(TypeParam{"foo"});
  } else {
    this->template onError<TypeParam>(TypeParam{"foo"});
  }

  if constexpr (std::is_same_v<TypeParam, std::string>) {
    ASSERT_EQ(std::get<std::vector<std::string>>(this->values),
              std::vector<std::string>{"foo"});
  } else {
    ASSERT_EQ(this->template errorMessages<TypeParam>(),
              std::vector<std::string>{"foo"});
  }
}

struct ValueTestBase : ::testing::Test {
  void SetUp() override {
    flw::ValueCallBacks<std::string, ErrorA, ErrorB> cb;
    cb.addOnValue([&store = this->store](const std::string &str) {
      store.template emplace<std::string>(str);
    });
    cb.template addOnError<ErrorA>([&store = this->store](const ErrorA &e) {
      store.template emplace<ErrorA>(e.what());
    });
    cb.template addOnError<ErrorB>([&store = this->store](const ErrorB &e) {
      store.template emplace<ErrorB>(e.what());
    });
    cb.template addOnError<std::exception>(
        [&store = this->store](const std::exception &e) {
          store.template emplace<std::runtime_error>(e.what());
        });
    val.emplace(std::move(cb));
  }

  std::variant<std::string, ErrorA, ErrorB, std::runtime_error> store;
  std::optional<flw::Value<std::string, ErrorA, ErrorB>> val;
};

TEST_F(ValueTestBase, onValue) {
  val->update([]() { return "hello world"; });
  ASSERT_TRUE(std::holds_alternative<std::string>(store));
  ASSERT_EQ(std::get<std::string>(store), "hello world");
}

template <typename ErrorT> std::string throw_exc() {
  throw ErrorT{"foo"};
  return "";
}

TEST_F(ValueTestBase, onError) {
  val->update(std::bind(&throw_exc<ErrorB>));
  ASSERT_TRUE(std::holds_alternative<ErrorB>(store));
  ASSERT_EQ(std::string{std::get<ErrorB>(store).what()}, "foo");
}

struct ErrorC : std::runtime_error {
  using std::runtime_error::runtime_error;
};

TEST_F(ValueTestBase, onErrorGeneric) {
  val->update(std::bind(&throw_exc<ErrorC>));
  ASSERT_TRUE(std::holds_alternative<std::runtime_error>(store));
  ASSERT_EQ(std::string{std::get<std::runtime_error>(store).what()}, "foo");
}
