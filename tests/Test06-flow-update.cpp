/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include <flow/Flow.h>
#include <sstream>

TEST(Flow, one_source_one_node_update) {
    flw::Flow flow;

    const std::string source_val = "Hello";
    const std::string node_val_addition = "World";

    auto source = flow.makeSource<std::string>("source");
    EXPECT_EQ(source.getGeneration(), 0);

    auto node = flow.makeNode("node",
        std::function<std::string(const std::string&)>([&node_val_addition](const std::string& input) { 
            std::stringstream stream;
            stream << input << node_val_addition;
            return stream.str();
            }) , source);
    EXPECT_EQ(node.getGeneration(), 0);
    
    flow.updateFlow(source.getName(), std::make_unique<std::string>(source_val));
    EXPECT_EQ(source.getGeneration(), 1);
    EXPECT_EQ(node.getGeneration(), 1);

    std::string expected = source_val;
    expected += node_val_addition;
    std::string from_node;
    EXPECT_TRUE(node.useValue([&from_node](const std::string& val) {
        from_node = val;
        }));

    EXPECT_EQ(expected, from_node);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
