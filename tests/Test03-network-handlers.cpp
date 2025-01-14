// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/
#include <gtest/gtest.h>

#include "Common.h"
#include <DynamicFlow/Error.h>
#include <DynamicFlow/Network.hxx>

#include <set>

struct HandlerMakerTest : public flw::HandlerMaker, public ::testing::Test {
  template <typename T>
  flw::HandlerSource<T> makeTestSource(T &&val, const std::string &label = "") {
    flw::test::IncrementGuard<1, std::vector<flw::FlowElementPtr>>
        guard_sources{this->sources_};
    flw::test::UnchangedGuard<std::vector<flw::FlowElementResettablePtr>>
        guard_nodes{this->nodes_};
    return this->makeSource<T>(std::forward<T>(val), label);
  }

  template <typename... As>
  flw::Handler<std::string> makeTestNode(const flw::Handler<As> &...deps,
                                         const std::string &label = "") {
    flw::test::UnchangedGuard<std::vector<flw::FlowElementPtr>> guard_sources{
        this->sources_};
    flw::test::IncrementGuard<1, std::vector<flw::FlowElementResettablePtr>>
        guard_nodes{this->nodes_};

    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    values.emplace_back(ptr);

    return this->makeNode<std::string, As...>(
        [](const As &...args) { return flw::detail::merge<0>(args...); },
        deps..., label, [ptr = ptr](const std::string &str) { *ptr = str; });
  }

  std::vector<std::shared_ptr<std::string>> values;
};

TEST_F(HandlerMakerTest, OneDependency_OneLevel_creation) {
  auto source = this->makeTestSource<int>(456);
  auto node = this->makeTestNode<int>(source);

  EXPECT_EQ(*values.front(), "456");
  EXPECT_TRUE(this->updatePending_.empty());
}

TEST_F(HandlerMakerTest, FourDependency_OneLevel_creation) {
  auto source1 = this->makeTestSource<int>(155);
  auto source2 = this->makeTestSource<std::string>("foo");
  auto source3 = this->makeTestSource<int>(250);
  auto source4 = this->makeTestSource<std::string>("bla");

  auto node = this->makeTestNode<int, std::string, int, std::string>(
      source1, source2, source3, source4);

  EXPECT_EQ(*values.front(), "155foo250bla");
  EXPECT_TRUE(this->updatePending_.empty());
}

TEST_F(HandlerMakerTest, TwoDependencies_TwoLevel_creation) {
  auto source_int = this->makeTestSource<int>(155);
  auto source_string = this->makeTestSource<std::string>("foo");

  auto node1 = this->makeTestNode<int, std::string>(source_int, source_string);

  EXPECT_EQ(*values.front(), "155foo");
  EXPECT_TRUE(this->updatePending_.empty());

  auto node2 =
      this->makeTestNode<std::string, std::string>(node1, source_string);
  EXPECT_EQ(*values.back(), "155foofoo");
  EXPECT_TRUE(this->updatePending_.empty());
}

TEST_F(HandlerMakerTest, SourceLabeled_creation) {
  const std::string source0_name = "source0";
  auto source0 = this->makeTestSource<int>(0, source0_name);

  const std::string source1_name = "source1";
  auto source1 = this->makeTestSource<int>(1, source1_name);

  std::set<std::string> keys, expected{"source0", "source1"};
  for (const auto &[k, _] : this->labeled_elements_) {
    keys.emplace(k);
  }
  EXPECT_EQ(keys, expected);
  EXPECT_EQ(this->sources_to_labels_.size(), 2);
  EXPECT_EQ(this->nodes_to_labels_.size(), 0);
}

TEST_F(HandlerMakerTest, NodeLabeled_creation) {
  auto source = this->makeTestSource<int>(0);

  const std::string node_name = "node";
  auto node0 = this->makeTestNode<int>(source, node_name);

  std::set<std::string> keys, expected{"node"};
  for (const auto &[k, _] : this->labeled_elements_) {
    keys.emplace(k);
  }
  EXPECT_EQ(keys, expected);
  EXPECT_EQ(this->sources_to_labels_.size(), 0);
  EXPECT_EQ(this->nodes_to_labels_.size(), 1);
}
