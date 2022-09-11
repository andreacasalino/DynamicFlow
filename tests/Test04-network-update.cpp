// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/
#include <gtest/gtest.h>

#include "ValueExtractor.h"
#include <DynamicFlow/Network.h>

#include <sstream>

namespace {
template <std::size_t Increment> class EpochIncrementGuard {
public:
  EpochIncrementGuard(const flw::Value &subject) : subject_(subject) {
    initial_epoch_ = subject_.epoch();
  }
  ~EpochIncrementGuard() {
    auto epoch = subject_.epoch();
    EXPECT_EQ(Increment, epoch - initial_epoch_);
  }

private:
  const flw::Value &subject_;
  std::size_t initial_epoch_;
};

using EpochUnchangedGuard = EpochIncrementGuard<0>;
using EpochSingleIncrementGuard = EpochIncrementGuard<1>;

std::string predicate_unary(const std::string &e) { return e; }

std::string predicate_binary(const std::string &a, const std::string &b) {
  std::stringstream stream;
  stream << a << b;
  auto retVal = stream.str();
  return retVal;
}
} // namespace

TEST(Flow, one_source_one_node_update) {
  flw::Flow flow;

  auto source = flow.makeSource<std::string>();

  auto node =
      flow.makeNode<std::string, std::string>(predicate_unary, "node", source);

  const std::string source_val = "Hello";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard node_guard(node.getValue());
    EpochSingleIncrementGuard source_guard(source.getValue());

    source.update(source_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node.getValue()), source_val);
}

TEST(Flow, two_source_one_node_update) {
  flw::Flow flow;

  auto source1 = flow.makeSource<std::string>();
  auto source2 = flow.makeSource<std::string>();

  auto node = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, "node", source1, source2);

  const std::string source1_val = "Hello";
  const std::string source2_val = "World";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard node_guard(node.getValue());
    EpochSingleIncrementGuard source1_guard(source1.getValue());
    EpochSingleIncrementGuard source2_guard(source2.getValue());

    source1.update(source1_val);
    source2.update(source2_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node.getValue()),
            source1_val + source2_val);
}

TEST(Flow, two_source_one_node_incomplete_update) {
  flw::Flow flow;

  auto source1 = flow.makeSource<std::string>();
  auto source2 = flow.makeSource<std::string>();

  auto node = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, "node", source1, source2);

  const std::string source1_val = "Hello";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochUnchangedGuard node_guard(node.getValue());
    EpochSingleIncrementGuard source1_guard(source1.getValue());
    EpochUnchangedGuard source2_guard(source2.getValue());

    source1.update(source1_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(node.getValue().status(), flw::ValueStatus::UNSET);
}

TEST(Flow, two_source_two_node_disjoint_update) {
  flw::Flow flow;

  auto source1 = flow.makeSource<std::string>();
  auto source2 = flow.makeSource<std::string>();

  auto node1 = flow.makeNode<std::string, std::string>(predicate_unary, "node1",
                                                       source1);
  auto node2 = flow.makeNode<std::string, std::string>(predicate_unary, "node2",
                                                       source2);

  const std::string source1_val = "Hello";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard node1_guard(node1.getValue());
    EpochSingleIncrementGuard source1_guard(source1.getValue());

    EpochUnchangedGuard node2_guard(node2.getValue());
    EpochUnchangedGuard source2_guard(source2.getValue());

    source1.update(source1_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node1.getValue()), source1_val);
  EXPECT_EQ(node2.getValue().status(), flw::ValueStatus::UNSET);

  const std::string source2_val = "World";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochUnchangedGuard node1_guard(node1.getValue());
    EpochUnchangedGuard source1_guard(source1.getValue());

    EpochSingleIncrementGuard node2_guard(node2.getValue());
    EpochSingleIncrementGuard source2_guard(source2.getValue());

    source2.update(source2_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node1.getValue()), source1_val);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node2.getValue()), source2_val);
}

TEST(Flow, two_source_two_node_joint_update) {
  flw::Flow flow;

  auto source1 = flow.makeSource<std::string>();
  auto source2 = flow.makeSource<std::string>();

  auto node1 = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, "node1", source1, source2);
  auto node2 = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, "node2", source1, source2);

  const std::string source1_val = "Hello";
  const std::string source2_val = "World";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard node1_guard(node1.getValue());
    EpochSingleIncrementGuard source1_guard(source1.getValue());

    EpochSingleIncrementGuard node2_guard(node2.getValue());
    EpochSingleIncrementGuard source2_guard(source2.getValue());

    source1.update(source1_val);
    source2.update(source2_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node1.getValue()),
            source1_val + source2_val);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node2.getValue()),
            source1_val + source2_val);
}

TEST(Flow, two_source_two_node_and_one_node_update) {
  flw::Flow flow;

  auto source1 = flow.makeSource<std::string>();
  auto source2 = flow.makeSource<std::string>();

  auto node1 = flow.makeNode<std::string, std::string>(predicate_unary,
                                                       std::nullopt, source1);
  auto node2 = flow.makeNode<std::string, std::string>(predicate_unary,
                                                       std::nullopt, source2);

  auto node3 = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, std::nullopt, node1, node2);

  const std::string source1_val = "Hello";
  const std::string source2_val = "World";
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard node1_guard(node1.getValue());
    EpochSingleIncrementGuard source1_guard(source1.getValue());

    EpochSingleIncrementGuard node2_guard(node2.getValue());
    EpochSingleIncrementGuard source2_guard(source2.getValue());

    EpochSingleIncrementGuard node3_guard(node3.getValue());

    source1.update(source1_val);
    source2.update(source2_val);
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node1.getValue()), source1_val);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node2.getValue()), source2_val);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node3.getValue()),
            source1_val + source2_val);
}

namespace {
class ExceptionTest : public flw::Error {
public:
  ExceptionTest() : flw::Error{""} {}
};

std::string predicate_with_exception(const std::string &e) {
  throw ExceptionTest{};
}
} // namespace

TEST(Flow, exception_in_update_fork) {
  flw::Flow flow;

  auto source = flow.makeSource<std::string>();

  auto node_1_val =
      std::make_unique<flw::ValueTypedWithErrors<std::string, ExceptionTest>>();
  auto node_1 = flow.makeNodeWithMonitoredException<std::string, std::string>(
      predicate_with_exception, std::move(node_1_val), std::nullopt, source);

  auto node_2_1 = flow.makeNode<std::string, std::string>(predicate_unary,
                                                          std::nullopt, node_1);
  auto node_2_2 = flow.makeNode<std::string, std::string>(predicate_unary,
                                                          std::nullopt, node_1);

  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard source_guard(source.getValue());
    EpochSingleIncrementGuard node_1_guard(node_1.getValue());
    EpochUnchangedGuard node_2_1_guard(node_2_1.getValue());
    EpochUnchangedGuard node_2_2_guard(node_2_2.getValue());

    source.update("");
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_THROW(node_1.getValue().reThrow(), ExceptionTest);
  EXPECT_EQ(node_2_1.getValue().status(), flw::ValueStatus::UNSET);
  EXPECT_EQ(node_2_2.getValue().status(), flw::ValueStatus::UNSET);
}

TEST(Flow, exception_in_update_join) {
  flw::Flow flow;

  auto source = flow.makeSource<std::string>();

  auto node_1_1 = flow.makeNode<std::string, std::string>(
      predicate_with_exception, std::nullopt, source);
  auto node_1_2 = flow.makeNode<std::string, std::string>(predicate_unary,
                                                          std::nullopt, source);

  auto node_2 = flow.makeNode<std::string, std::string, std::string>(
      predicate_binary, std::nullopt, node_1_1, node_1_2);

  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  {
    EpochSingleIncrementGuard source_guard(source.getValue());
    EpochSingleIncrementGuard node_1_1_guard(node_1_1.getValue());
    EpochSingleIncrementGuard node_1_2_guard(node_1_2.getValue());
    EpochUnchangedGuard node_2_guard(node_2.getValue());

    source.update("");
    flow.update();
  }
  EXPECT_EQ(flow.status(), flw::FlowStatus::IDLE);
  EXPECT_THROW(node_1_1.getValue().reThrow(), flw::Error);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node_1_2.getValue()), "");
  EXPECT_EQ(node_2.getValue().status(), flw::ValueStatus::UNSET);
}