/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include <flow/Source.hpp>
#include <flow/Node.hpp>

template<typename T>
class SourceTest
    : public flw::Source<T> {
public:
    SourceTest()
        : flw::Source<T>("") {
    };
};

template<typename T, typename ... Ts>
class NodeTest
    : public flw::Node<T, Ts...> {
public:
    template<typename ... Values>
    NodeTest(const std::function<T(const Ts & ...)>& evaluation, const Values& ... ancestors)
        : flw::Node<T, Ts...>("", evaluation) {
        bind(ancestors...);
        subscribe(ancestors...);
    };
};

TEST(SourcesNodes, oneSources_oneNode) {
    SourceTest<int> source;
    NodeTest<int, int> node([](const int& input) { return 0; }, source);

    const auto* anc = node.getAncestor<0>().ancestor;
    EXPECT_EQ(anc, &source);
    EXPECT_EQ(source.descendants.size(), 1);
    EXPECT_EQ(source.descendants.front(), &node);
}

TEST(SourcesNodes, twoSources_oneNode) {
    SourceTest<int> source0;
    SourceTest<float> source1;
    NodeTest<int, int, float> node([](const int& i1, const float& i2) { return 0; }, source0, source1);

    const auto* anc0 = node.getAncestor<0>().ancestor;
    EXPECT_EQ(anc0, &source0);

    const auto* anc1 = node.getAncestor<1>().ancestor;
    EXPECT_EQ(anc1, &source1);

    EXPECT_EQ(source0.descendants.size(), 1);
    EXPECT_EQ(source0.descendants.front(), &node);
    EXPECT_EQ(source1.descendants.size(), 1);
    EXPECT_EQ(source1.descendants.front(), &node);
}

TEST(SourcesNodes, twoSources_twoNode) {
    SourceTest<int> source0;
    SourceTest<float> source1;

    NodeTest<int, int, float> node0([](const int& i1, const float& i2) { return 0; }, source0, source1);
    NodeTest<int, int, float> node1([](const int& i1, const float& i2) { return 0; }, source0, source1);

    {
        const auto* anc0 = node0.getAncestor<0>().ancestor;
        EXPECT_EQ(anc0, &source0);

        const auto* anc1 = node0.getAncestor<1>().ancestor;
        EXPECT_EQ(anc1, &source1);
    }

    {
        const auto* anc0 = node1.getAncestor<0>().ancestor;
        EXPECT_EQ(anc0, &source0);

        const auto* anc1 = node1.getAncestor<1>().ancestor;
        EXPECT_EQ(anc1, &source1);
    }

    EXPECT_EQ(source0.descendants.size(), 2);
    EXPECT_EQ(source0.descendants.front(), &node0);
    EXPECT_EQ(source0.descendants.back(), &node1);

    EXPECT_EQ(source1.descendants.size(), 2);
    EXPECT_EQ(source1.descendants.front(), &node0);
    EXPECT_EQ(source1.descendants.back(), &node1);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
