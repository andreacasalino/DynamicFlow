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

    flow.makeSource<int>("source");
    auto source = flow.findSource<int>("source");
    EXPECT_FALSE(source.isValue());
    EXPECT_FALSE(source.isException());
}

//TEST(Flow, source_copy) {
//    flw::Flow flow;
//
//    auto source = flow.makeSource<int>("source");
//
//    auto source_copy = source;
//    EXPECT_FALSE(source_copy.isValue());
//    EXPECT_FALSE(source_copy.isException());
//}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
