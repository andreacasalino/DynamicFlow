/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>

void merge(std::stringstream &stream, const std::string &toAdd) {
  stream << toAdd;
};

template <typename... Args>
void merge(std::stringstream &stream, const std::string &toAdd,
           const Args &...remaining) {
  merge(stream, toAdd);
  merge(stream, remaining...);
};

template <typename... Args>
std::string merge(const std::string &toAdd, const Args &...remaining) {
  std::stringstream stream;
  merge(stream, toAdd, remaining...);
  return stream.str();
};

#define MERGE_SINGLE                                                           \
  std::function<std::string(const std::string &input)>(                        \
      [](const std::string &input) -> std::string { return input; })

#define MERGE_DUAL                                                             \
  std::function<std::string(const std::string &input1,                         \
                            const std::string &input2)>(                       \
      [](const auto &...inputs) -> std::string { return merge(inputs...); })

template <typename NodeT> std::string copyValue(NodeT &node) {
  std::string copy;
  EXPECT_TRUE(node.useValue([&copy](const std::string &val) { copy = val; }));
  return copy;
}

TEST(Flow, one_source_one_node_update) {
  flw::Flow flow;

  const std::string source_val = "Hello";

  auto source = flow.makeSource<std::string>("source");
  EXPECT_EQ(source.getGeneration(), 0);

  auto node = flow.makeNode("node", MERGE_SINGLE, source);
  EXPECT_EQ(node.getGeneration(), 0);

  flow.updateSourcesAndFlow(source.getName(),
                            std::make_unique<std::string>(source_val));
  EXPECT_EQ(source.getGeneration(), 1);
  EXPECT_EQ(node.getGeneration(), 1);

  EXPECT_EQ(copyValue(node), source_val);
}

TEST(Flow, two_source_one_node_update) {
  flw::Flow flow;

  const std::string source_val1 = "Hello";
  const std::string source_val2 = "World";

  auto source1 = flow.makeSource<std::string>("source1");
  EXPECT_EQ(source1.getGeneration(), 0);
  auto source2 = flow.makeSource<std::string>("source2");
  EXPECT_EQ(source2.getGeneration(), 0);

  auto node = flow.makeNode("node", MERGE_DUAL, source1, source2);
  EXPECT_EQ(node.getGeneration(), 0);

  flow.updateSourcesAndFlow(
      source1.getName(), std::make_unique<std::string>(source_val1),
      source2.getName(), std::make_unique<std::string>(source_val2));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(source2.getGeneration(), 1);
  EXPECT_EQ(node.getGeneration(), 1);

  EXPECT_EQ(copyValue(node), merge(source_val1, source_val2));
}

TEST(Flow, two_source_one_node_incomplete_update) {
  flw::Flow flow;

  const std::string source_val1 = "Hello";
  const std::string source_val2 = "World";

  auto source1 = flow.makeSource<std::string>("source1");
  EXPECT_EQ(source1.getGeneration(), 0);
  auto source2 = flow.makeSource<std::string>("source2");
  EXPECT_EQ(source2.getGeneration(), 0);

  auto node = flow.makeNode("node", MERGE_DUAL, source1, source2);
  EXPECT_EQ(node.getGeneration(), 0);

  flow.updateSourcesAndFlow(source1.getName(),
                            std::make_unique<std::string>(source_val1));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(source2.getGeneration(), 0);
  EXPECT_EQ(node.getGeneration(), 0);
}

TEST(Flow, two_source_two_node_disjoint_update) {
  flw::Flow flow;

  const std::string source_val1 = "Hello";
  const std::string source_val2 = "World";

  auto source1 = flow.makeSource<std::string>("source1");
  auto source2 = flow.makeSource<std::string>("source2");

  auto node1 = flow.makeNode("node1", MERGE_SINGLE, source1);
  auto node2 = flow.makeNode("node2", MERGE_SINGLE, source2);

  flow.updateSourcesAndFlow(source1.getName(),
                            std::make_unique<std::string>(source_val1));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(node1.getGeneration(), 1);
  EXPECT_EQ(copyValue(node1), source_val1);
  EXPECT_EQ(source2.getGeneration(), 0);
  EXPECT_EQ(node2.getGeneration(), 0);

  flow.updateSourcesAndFlow(source2.getName(),
                            std::make_unique<std::string>(source_val2));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(node1.getGeneration(), 1);
  EXPECT_EQ(source2.getGeneration(), 1);
  EXPECT_EQ(node2.getGeneration(), 1);
  EXPECT_EQ(copyValue(node2), source_val2);
}

TEST(Flow, two_source_two_node_joint_update) {
  flw::Flow flow;

  const std::string source_val1 = "Hello";
  const std::string source_val2 = "World";

  auto source1 = flow.makeSource<std::string>("source1");
  auto source2 = flow.makeSource<std::string>("source2");

  auto node1 = flow.makeNode("node1", MERGE_DUAL, source1, source2);
  auto node2 = flow.makeNode("node2", MERGE_DUAL, source2, source1);

  flow.updateSourcesAndFlow(
      source1.getName(), std::make_unique<std::string>(source_val1),
      source2.getName(), std::make_unique<std::string>(source_val2));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(node1.getGeneration(), 1);
  EXPECT_EQ(source2.getGeneration(), 1);
  EXPECT_EQ(node2.getGeneration(), 1);
  EXPECT_EQ(copyValue(node1), merge(source_val1, source_val2));
  EXPECT_EQ(copyValue(node2), merge(source_val2, source_val1));
}

TEST(Flow, two_source_three_node_update) {
  flw::Flow flow;

  const std::string source_val1 = "Hello";
  const std::string source_val2 = "World";

  auto source1 = flow.makeSource<std::string>("source1");
  auto source2 = flow.makeSource<std::string>("source2");

  auto node1 = flow.makeNode("node1", MERGE_DUAL, source1, source2);
  auto node2 = flow.makeNode("node2", MERGE_DUAL, source2, source1);

  auto node3 = flow.makeNode("node3", MERGE_DUAL, node1, node2);

  flow.updateSourcesAndFlow(
      source1.getName(), std::make_unique<std::string>(source_val1),
      source2.getName(), std::make_unique<std::string>(source_val2));
  EXPECT_EQ(source1.getGeneration(), 1);
  EXPECT_EQ(node1.getGeneration(), 1);
  EXPECT_EQ(source2.getGeneration(), 1);
  EXPECT_EQ(node2.getGeneration(), 1);
  EXPECT_EQ(node3.getGeneration(), 1);
  EXPECT_EQ(copyValue(node1), merge(source_val1, source_val2));
  EXPECT_EQ(copyValue(node2), merge(source_val2, source_val1));
  EXPECT_EQ(copyValue(node3), merge(copyValue(node1), copyValue(node2)));
}

TEST(Flow, node_creation_while_updating_flow) {
  flw::Flow flow;

  auto source = flow.makeSource<int>("source");
  auto node = flow.makeNode(
      "node", std::function<int(const int &)>([](const int &input) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return input;
      }),
      source);

  flow.updateSourcesAndFlow(source.getName(), std::make_unique<int>(0));
  {
    std::thread th([&]() {
      flow.makeNode("node2",
                    std::function<int(const int &)>(
                        [](const int &input) { return input; }),
                    node);
    });
    th.join();
  }
  auto node2 = flow.findNode<int>("node2");

  flow.waitUpdateComplete();

  EXPECT_TRUE(node.isValue());
  EXPECT_FALSE(node2.isValue());
  auto node_old_gen = node.getGeneration();
  auto node2_old_gen = node2.getGeneration();

  flow.updateFlow();
  flow.waitUpdateComplete();
  EXPECT_TRUE(node.isValue());
  EXPECT_TRUE(node2.isValue());

  EXPECT_EQ(node_old_gen, node.getGeneration());
  EXPECT_EQ(node2_old_gen + 1, node2.getGeneration());
}

class FlowTest : public flw::Flow {
public:
  FlowTest() {
    auto source = makeSource<std::string>(source_name);
    makeNode(nodeA_name, copier, source);
    makeNode(nodeB_name, copier, source);
  }

  void resetSource() {
    updateSource(source_name, std::make_unique<std::string>(source_content));
  }

  void expect_empty() const {
    EXPECT_TRUE(sources.empty());
    EXPECT_TRUE(nodes.empty());
    EXPECT_TRUE(allTogether.empty());
    EXPECT_TRUE(requiringUpdate.empty());
  }

  void expect_non_empty() const {
    EXPECT_FALSE(sources.empty());
    EXPECT_FALSE(nodes.empty());
    EXPECT_FALSE(allTogether.empty());
    EXPECT_FALSE(requiringUpdate.empty());
  }

  static const std::string source_name;
  static const std::string source_content;
  static const std::string nodeA_name;
  static const std::string nodeB_name;

private:
  static const std::function<std::string(const std::string &)> copier;
};
const std::string FlowTest::source_name = "source";
const std::string FlowTest::source_content = "Hello World";
const std::string FlowTest::nodeA_name = "nodeA";
const std::string FlowTest::nodeB_name = "nodeB";
const std::function<std::string(const std::string &)> FlowTest::copier =
    [](const auto &in) { return in; };

TEST(Flow, move_semantic) {
  {
    FlowTest giver;
    giver.expect_non_empty();

    FlowTest recipient;
    recipient.absorb(std::move(giver));
    giver.expect_empty();
    recipient.expect_non_empty();

    EXPECT_THROW(giver.resetSource(), flw::Error);
    recipient.resetSource();
  }

  {
    FlowTest giver;

    auto source = giver.findSource<std::string>(FlowTest::source_name);
    auto nodeA = giver.findNode<std::string>(FlowTest::nodeA_name);
    auto nodeB = giver.findNode<std::string>(FlowTest::nodeB_name);

    FlowTest recipient;
    recipient.absorb(std::move(giver));

    EXPECT_THROW(giver.findSource<std::string>(FlowTest::source_name),
                 flw::Error);

    recipient.resetSource();
    recipient.updateFlow();

    EXPECT_TRUE(source.isValue());
    EXPECT_EQ(flw::copyValue(source), FlowTest::source_content);
    EXPECT_TRUE(nodeA.isValue());
    EXPECT_EQ(flw::copyValue(nodeA), FlowTest::source_content);
    EXPECT_TRUE(nodeB.isValue());
    EXPECT_EQ(flw::copyValue(nodeB), FlowTest::source_content);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
