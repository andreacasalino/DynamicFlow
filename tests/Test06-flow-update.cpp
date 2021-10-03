/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include <flow/Flow.h>
#include <sstream>

//template<typename ... Args>
//std::string merge(const std::string& toAdd, Args ... remaining) {
//    std::stringstream stream;
//    merge(stream, toAdd, remaining...);
//    return stream.str();
//};
//
//template<typename ... Args>
//void merge(std::stringstream& stream, const std::string& toAdd, Args ... remaining) {
//    merge(stream, toAdd);
//    merge(stream, remaining...);
//};
//
//void merge(std::stringstream& stream, const std::string& toAdd) {
//    stream << toAdd;
//};
//
//#define MERGE_SINGLE \
//std::function<std::string(const std::string & input)>( \
//[](const std::string& input) -> std::string { \
//    return input; \
//})
//
//#define MERGE_DUAL \
//std::function<std::string(const std::string & input1, const std::string & input2)>( \
//[](const auto& ... inputs) -> std::string { \
//    return merge(inputs...); \
//})
//
//template<typename NodeT>
//std::string copyValue(NodeT& node) {
//    std::string copy;
//    EXPECT_TRUE(node.useValue([&copy](const std::string& val) {
//        copy = val;
//    }));
//    return copy;
//}
//
//TEST(Flow, one_source_one_node_update) {
//    flw::Flow flow;
//
//    const std::string source_val = "Hello";
//
//    auto source = flow.makeSource<std::string>("source");
//    EXPECT_EQ(source.getGeneration(), 0);
//
//    auto node = flow.makeNode("node", MERGE_SINGLE, source);
//    EXPECT_EQ(node.getGeneration(), 0);
//    
//    flow.updateFlow(source.getName(), std::make_unique<std::string>(source_val));
//    EXPECT_EQ(source.getGeneration(), 1);
//    EXPECT_EQ(node.getGeneration(), 1);
//
//    EXPECT_EQ(copyValue(node), source_val);
//}
//
//TEST(Flow, two_source_one_node_update) {
//    flw::Flow flow;
//
//    const std::string source_val1 = "Hello";
//    const std::string source_val2 = "World";
//
//    auto source1 = flow.makeSource<std::string>("source1");
//    EXPECT_EQ(source1.getGeneration(), 0);
//    auto source2 = flow.makeSource<std::string>("source2");
//    EXPECT_EQ(source2.getGeneration(), 0);
//
//    auto node = flow.makeNode("node", MERGE_DUAL, source1, source2);
//    EXPECT_EQ(node.getGeneration(), 0);
//
//    flow.updateFlow(source1.getName(), std::make_unique<std::string>(source_val1),
//                    source2.getName(), std::make_unique<std::string>(source_val2));
//    EXPECT_EQ(source1.getGeneration(), 1);
//    EXPECT_EQ(source2.getGeneration(), 1);
//    EXPECT_EQ(node.getGeneration(), 1);
//
//    EXPECT_EQ(copyValue(node), merge(source_val1, source_val2));
//}
//
//TEST(Flow, two_source_two_node_disjoint_update) {
//    flw::Flow flow;
//
//    const std::string source_val1 = "Hello";
//    const std::string source_val2 = "World";
//
//    auto source1 = flow.makeSource<std::string>("source1");
//    auto source2 = flow.makeSource<std::string>("source2");
//
//    auto node1 = flow.makeNode("node1", MERGE_SINGLE, source1);
//    auto node2 = flow.makeNode("node2", MERGE_SINGLE, source2);
//
//    flow.updateFlow(source1.getName(), std::make_unique<std::string>(source_val1));
//    EXPECT_EQ(source1.getGeneration(), 1);
//    EXPECT_EQ(node1.getGeneration(), 1);
//    EXPECT_EQ(copyValue(node1), source_val1);
//    EXPECT_EQ(source2.getGeneration(), 0);
//    EXPECT_EQ(node2.getGeneration(), 0);
//
//    flow.updateFlow(source2.getName(), std::make_unique<std::string>(source_val2));
//    EXPECT_EQ(source1.getGeneration(), 1);
//    EXPECT_EQ(node1.getGeneration(), 1);
//    EXPECT_EQ(source2.getGeneration(), 1);
//    EXPECT_EQ(node2.getGeneration(), 1);
//    EXPECT_EQ(copyValue(node2), source_val2);
//}
//
//TEST(Flow, two_source_two_node_joint_update) {
//    flw::Flow flow;
//
//    const std::string source_val1 = "Hello";
//    const std::string source_val2 = "World";
//
//    auto source1 = flow.makeSource<std::string>("source1");
//    auto source2 = flow.makeSource<std::string>("source2");
//
//    auto node1 = flow.makeNode("node1", MERGE_DUAL, source1, source2);
//    auto node2 = flow.makeNode("node2", MERGE_DUAL, source2, source1);
//
//    flow.updateFlow(source1.getName(), std::make_unique<std::string>(source_val1));
//    EXPECT_EQ(source1.getGeneration(), 1);
//    EXPECT_EQ(node1.getGeneration(), 1);
//    EXPECT_EQ(source2.getGeneration(), 1);
//    EXPECT_EQ(node2.getGeneration(), 1);
//    EXPECT_EQ(copyValue(node1), merge(source_val1, source_val2));
//    EXPECT_EQ(copyValue(node1), merge(source_val2, source_val1));
//}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
