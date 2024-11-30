/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/Node.hxx>
#include "Common.h"

#include <sstream>


template <typename... As> struct NodeTest : ::testing::Test {
  using CB = flw::ValueCallBacks<std::string, flw::test::ErrorTest>;

  template <bool Throw> void setUp(As &&...initialValues) {
    this->setUp_<Throw>(std::forward<As>(initialValues)..., CB{});
  }

  template <bool Throw, typename onSucess, typename onException>
  void setUp(As &&...initialValues, onSucess &&succ, onException &&err) {
    CB cb;
    cb.addOnValue(std::forward<onSucess>(succ));
    cb.addOnError<flw::test::ErrorTest>(std::forward<onException>(err));
    this->setUp_<Throw>(std::forward<As>(initialValues)..., std::move(cb));
  }

  std::tuple<flw::NodeBasePtr<As>...> sources;
  std::shared_ptr<flw::Node<std::string, As...>> node;

private:
  template <std::size_t Index, typename Afront, typename... Arest>
  void setUpSource_(Afront &&front, Arest &&...rest) {
    auto &ref = std::get<Index>(sources);
    ref = std::make_shared<flw::Source<Afront>>(std::forward<Afront>(front));
    if constexpr (0 < sizeof...(Arest)) {
      setUpSource_<Index + 1, Arest...>(std::forward<Arest>(rest)...);
    }
  }

  template <bool Throw> void setUp_(As &&...initialValues, CB &&cb) {
    this->setUpSource_<0, As...>(std::forward<As>(initialValues)...);
    node = flw::Node<std::string, As...>::make(
        [](const As &...vals) {
          if constexpr (Throw) {
            throw flw::test::ErrorTest{""};
          }
          return flw::detail::merge<0>(vals...);
        },
        sources, std::forward<CB>(cb));
  }
};

struct NodeTest_OneSource : NodeTest<int> {
  void SetUp() override { this->setUp<false>(int{306}); }
};

TEST_F(NodeTest_OneSource, update) {
  node->update();
  ASSERT_TRUE(node->value()->getValue());
  EXPECT_EQ(*node->value()->getValue(), "306");
}

TEST_F(NodeTest_OneSource, reset) {
  node->update();
  node->reset();
  ASSERT_FALSE(node->value()->getValue());
}

struct NodeTest_MultipleSources : NodeTest<int, std::string, int> {
  void SetUp() override {
    this->setUp<false>(int{306}, "hello world", int{603});
  }
};

TEST_F(NodeTest_MultipleSources, update) {
  node->update();
  ASSERT_TRUE(node->value()->getValue());
  EXPECT_EQ(*node->value()->getValue(), "306hello world603");
}

struct NodeTest_MultipleSources_withCB : NodeTest<int, std::string, int> {
  template <bool Throw> void doSetUp() {
    this->setUp<Throw>(
        int{306}, "hello world", int{603},
        [this](const std::string &str) { val.emplace<std::string>(str); },
        [this](const flw::test::ErrorTest &) { val.emplace<flw::test::ErrorTest>(""); });
  }

  struct None {};
  std::variant<None, std::string, flw::test::ErrorTest> val;
};

TEST_F(NodeTest_MultipleSources_withCB, update_with_cb_sucess) {
  this->doSetUp<false>();

  node->update();
  ASSERT_TRUE(node->value()->getValue());
  EXPECT_EQ(*node->value()->getValue(), "306hello world603");
  ASSERT_TRUE(std::holds_alternative<std::string>(this->val));
}

TEST_F(NodeTest_MultipleSources_withCB, update_with_cb_throw) {
  this->doSetUp<true>();

  node->update();
  ASSERT_FALSE(node->value()->getValue());
  ASSERT_TRUE(std::holds_alternative<flw::test::ErrorTest>(this->val));
}
