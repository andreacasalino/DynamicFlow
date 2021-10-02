/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include <flow/Flow.h>

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
        auto node = flow.template makeNode<int, int>("node", [](const int& input) { return input; }, source);
    }

    {
        flw::Flow flow;

        auto source1 = flow.makeSource<int>("source1");
        auto source2 = flow.makeSource<float>("source2");
        auto node = flow.template makeNode<int, int, float>("node", [](const int& in1, const float& in2) -> int { return in1; }, source1, source2);

        auto node2 = flow.template makeNode<int, int, float>("node2", [](const int& in1, const float& in2) -> int { return in1; }, node, source2);
    }
}

//TEST(Flow, node_search) {
//    flw::Flow flow;
//
//    const std::string node_name = "node";
//
//    {
//        auto source1 = flow.makeSource<int>("source1");
//        auto source2 = flow.makeSource<float>("source2");
//        flow.makeNode<int, int, float>(node_name, [](const int& in1, const float& in2) -> int { return in1; }, source1, source2);
//    }
//
//    auto node = flow.findNode<int, int, float>(node_name);
//    EXPECT_TRUE(node.getName().compare(node_name) == 0);
//    EXPECT_FALSE(node.isValue());
//    EXPECT_FALSE(node.isException());
//}
//
//TEST(Flow, node_copy) {
//    flw::Flow flow;
//
//    auto source1 = flow.makeSource<int>("source1");
//    auto source2 = flow.makeSource<float>("source2");
//    auto node = flow.makeNode<int, int, float>("node", [](const int& in1, const float& in2) -> int { return in1; }, source1, source2);
//
//    auto node_copy = node;
//    EXPECT_FALSE(node_copy.isValue());
//    EXPECT_FALSE(node_copy.isException());
//}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
