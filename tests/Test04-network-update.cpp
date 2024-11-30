// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/
#include <gtest/gtest.h>

#include "Common.h"
#include <DynamicFlow/Network.hxx>

#include <mutex>

struct UpdateTest : ::testing::Test {
  flw::Flow flow;

  struct Info {
    std::string value;
    std::size_t epoch = 0;
  };
  std::vector<Info> values;

  void set(std::size_t index, const std::string &val) {
    values[index].value = val;
    ++values[index].epoch;
  }

  const auto &at(std::size_t index) const { return values.at(index); }

  template <bool Throw>
  flw::Handler<std::string> makeNode(const flw::Handler<std::string> &s) {
    values.emplace_back();
    return flow.makeNode<std::string, std::string>(
        [lam = flw::test::ComposerLambda<Throw>{}](const std::string &val) {
          return lam(val);
        },
        s, "",
        std::bind(&UpdateTest::set, std::ref(*this), values.size() - 1,
                  std::placeholders::_1));
  }

  template <bool Throw>
  flw::Handler<std::string> makeNode(const flw::Handler<std::string> &s0,
                                     const flw::Handler<std::string> &s1) {
    values.emplace_back();
    return flow.makeNode<std::string, std::string, std::string>(
        [lam = flw::test::ComposerLambda<Throw>{}](
            const std::string &a, const std::string &b) { return lam(a, b); },
        s0, s1, "",
        std::bind(&UpdateTest::set, std::ref(*this), values.size() - 1,
                  std::placeholders::_1));
  }
};

TEST_F(UpdateTest, one_source_one_node_update) {
  auto source = flow.makeSource<std::string>("Hello");

  auto node = this->makeNode<false>(source);

  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "Hello");
  EXPECT_EQ(at(0).epoch, 1);
}

TEST_F(UpdateTest, one_source_one_node_update_not_needed_as_same_val) {
  auto source = flow.makeSource<std::string>("Hello");

  auto node = this->makeNode<false>(source);

  source.update("Hello");
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "Hello");
  EXPECT_EQ(at(0).epoch, 1);
}

TEST_F(UpdateTest, one_source_one_node_update_deferred) {
  auto source = flow.makeSource<std::string>("Hello");

  flow.setOnNewNodePolicy(flw::HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE);
  auto node = this->makeNode<false>(source);

  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_REQUIRED);

  flow.update();
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "Hello");
  EXPECT_EQ(at(0).epoch, 1);
}

TEST_F(UpdateTest, two_source_one_node_update) {
  auto source1 = flow.makeSource<std::string>("Hello");
  auto source2 = flow.makeSource<std::string>("World");

  auto node = this->makeNode<false>(source1, source2);

  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "HelloWorld");
  EXPECT_EQ(at(0).epoch, 1);

  {
    SCOPED_TRACE("Update source 1");

    source1.update("Ciao");
    ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_REQUIRED);

    flow.update();
    ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
    EXPECT_EQ(at(0).value, "CiaoWorld");
    EXPECT_EQ(at(0).epoch, 2);
  }

  {
    SCOPED_TRACE("Update source 2");

    source2.update("Mondo");
    ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_REQUIRED);

    flow.update();
    ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
    EXPECT_EQ(at(0).value, "CiaoMondo");
    EXPECT_EQ(at(0).epoch, 3);
  }
}

TEST_F(UpdateTest, two_source_one_node_update_not_needed_as_same_val) {
  auto source1 = flow.makeSource<std::string>("Hello");
  auto source2 = flow.makeSource<std::string>("World");

  auto node = this->makeNode<false>(source1, source2);

  source1.update("Hello");
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "HelloWorld");
  EXPECT_EQ(at(0).epoch, 1);

  source2.update("World");
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "HelloWorld");
  EXPECT_EQ(at(0).epoch, 1);
}

TEST_F(UpdateTest, two_source_two_node_joint_update) {
  auto source1 = flow.makeSource<std::string>("Hello");
  auto source2 = flow.makeSource<std::string>("World");

  auto node1 = this->makeNode<false>(source1, source2);
  auto node2 = this->makeNode<false>(source1, source2);

  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  for (std::size_t k = 0; k < 2; ++k) {
    EXPECT_EQ(at(k).value, "HelloWorld");
    EXPECT_EQ(at(k).epoch, 1);
  }
}

TEST_F(UpdateTest, two_source_two_node_and_one_node_update) {
  auto source1 = flow.makeSource<std::string>("Hello");
  auto source2 = flow.makeSource<std::string>("World");

  auto node1 = this->makeNode<false>(source1, source2);
  auto node2 = this->makeNode<false>(source1, source2);

  auto node3 = this->makeNode<false>(node1, node2);

  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).value, "HelloWorld");
  EXPECT_EQ(at(0).epoch, 1);
  EXPECT_EQ(at(1).value, "HelloWorld");
  EXPECT_EQ(at(1).epoch, 1);
  EXPECT_EQ(at(2).value, "HelloWorldHelloWorld");
  EXPECT_EQ(at(2).epoch, 1);
}

TEST_F(UpdateTest, one_source_one_node_then_fork_update_with_exception) {
  auto source = flow.makeSource<std::string>("Hello");

  auto node_1 = this->makeNode<true>(source); // first time exception is thrown

  auto node_2_1 = this->makeNode<false>(node_1);
  auto node_2_2 = this->makeNode<false>(node_1);

  for (auto &[_, epoch] : values) {
    EXPECT_EQ(at(0).epoch, 0);
  }

  source.update("Helloo");
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_REQUIRED);

  flow.update();
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  for (auto &[val, epoch] : values) {
    EXPECT_EQ(at(0).value, "Helloo");
    EXPECT_EQ(at(0).epoch, 1);
  }
}

TEST_F(UpdateTest, one_source_then_fork_then_sink_update_with_exception) {
  auto source = flow.makeSource<std::string>("Hello");

  auto node_1_1 =
      this->makeNode<true>(source); // first time exception is thrown
  auto node_1_2 =
      this->makeNode<false>(source); // first time exception is thrown

  auto node_2 = this->makeNode<false>(node_1_1, node_1_2);

  EXPECT_EQ(at(0).epoch, 0);
  EXPECT_EQ(at(1).value, "Hello");
  EXPECT_EQ(at(1).epoch, 1);
  EXPECT_EQ(at(2).epoch, 0);

  source.update("Helloo");

  flow.update();
  ASSERT_EQ(flow.status(), flw::FlowStatus::UPDATE_NOT_REQUIRED);
  EXPECT_EQ(at(0).epoch, 1);
  EXPECT_EQ(at(0).value, "Helloo");
  EXPECT_EQ(at(1).epoch, 2);
  EXPECT_EQ(at(1).value, "Helloo");
  EXPECT_EQ(at(2).epoch, 1);
  EXPECT_EQ(at(2).value, "HellooHelloo");
}
