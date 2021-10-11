/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>
#include <gtest/gtest.h>

TEST(Flow, source_creation) {
  flw::Flow flow;

  auto source1 = flow.makeSource<int>("source1");
  EXPECT_FALSE(source1.isValue());
  EXPECT_FALSE(source1.isException());
}

TEST(Flow, source_search) {
  flw::Flow flow;

  const std::string source_name = "source";

  flow.makeSource<int>(source_name);
  auto source = flow.findSource<int>(source_name);
  EXPECT_TRUE(source.getName().compare(source_name) == 0);
  EXPECT_FALSE(source.isValue());
  EXPECT_FALSE(source.isException());
}

TEST(Flow, source_copy) {
  flw::Flow flow;

  auto source = flow.makeSource<int>("source");

  auto source_copy = source;
  EXPECT_FALSE(source_copy.isValue());
  EXPECT_FALSE(source_copy.isException());
}

TEST(Flow, node_creation) {
  {
    flw::Flow flow;

    auto source = flow.makeSource<int>("source");
    auto node = flow.makeNode(
        "node",
        std::function<int(const int &)>([](const int &input) { return input; }),
        source);
  }

  {
    flw::Flow flow;

    auto source1 = flow.makeSource<int>("source1");
    auto source2 = flow.makeSource<float>("source2");
    auto node =
        flow.makeNode("node",
                      std::function<int(const int &, const float &)>(
                          [](const int &in1, const float &in2) { return in1; }),
                      source1, source2);

    auto node2 = flow.makeNode(
        "node2",
        std::function<int(const int &, const float &)>(
            [](const int &in1, const float &in2) -> int { return in1; }),
        node, source2);
  }
}

TEST(Flow, node_search) {
  flw::Flow flow;

  const std::string node_name = "node";

  {
    auto source1 = flow.makeSource<int>("source1");
    auto source2 = flow.makeSource<float>("source2");
    flow.makeNode(
        node_name,
        std::function<int(const int &, const float &)>(
            [](const int &in1, const float &in2) -> int { return in1; }),
        source1, source2);
  }

  auto node = flow.findNode<int>(node_name);
  EXPECT_TRUE(node.getName().compare(node_name) == 0);
  EXPECT_FALSE(node.isValue());
  EXPECT_FALSE(node.isException());
}

TEST(Flow, node_copy) {
  flw::Flow flow;

  auto source1 = flow.makeSource<int>("source1");
  auto source2 = flow.makeSource<float>("source2");
  auto node = flow.makeNode(
      "node",
      std::function<int(const int &, const float &)>(
          [](const int &in1, const float &in2) -> int { return in1; }),
      source1, source2);

  auto node_copy = node;
  EXPECT_FALSE(node_copy.isValue());
  EXPECT_FALSE(node_copy.isException());
}

TEST(Flow, node_name_already_existing) {
  flw::Flow flow;

  auto source1 = flow.makeSource<int>("source1");
  auto source2 = flow.makeSource<float>("source2");

  flow.makeNode(
      "node",
      std::function<int(const int &, const float &)>(
          [](const int &in1, const float &in2) -> int { return in1; }),
      source1, source2);

  ASSERT_THROW(flow.makeNode("node",
                             std::function<int(const int &, const float &)>(
                                 [](const int &in1, const float &in2) -> int {
                                   return in1;
                                 }),
                             source1, source2),
               flw::Error);
}

TEST(Flow, node_external_input) {
  flw::Flow flow1;
  auto source1 = flow1.makeSource<int>("source1");

  flw::Flow flow2;
  auto source2 = flow2.makeSource<float>("source2");

  ASSERT_THROW(flow1.makeNode("node",
                              std::function<int(const int &, const float &)>(
                                  [](const int &in1, const float &in2) -> int {
                                    return in1;
                                  }),
                              source1, source2),
               flw::Error);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
