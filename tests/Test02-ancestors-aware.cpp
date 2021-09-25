/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include "ValueStorerTest.hpp"
#include <components/AncestorsAware.hpp>
#include <string>

template<typename ... Ts>
class AncestorsAwareTest
    : public flw::AncestorsAware<Ts...> {
public:
    template<typename ... Values>
    AncestorsAwareTest(const Values& ... ancestors) {
        bind(ancestors...);
    };
};

TEST(AncestorsAware, triple_bind_check) {
    ValueStorerTest<int> val0;
    ValueStorerTest<float> val1;
    ValueStorerTest<int> val2;

    AncestorsAwareTest<int, float, int> sample(val0, val1, val2);

    auto* val0_pt = sample.getAncestor<0>().ancestor;
    EXPECT_TRUE(val0_pt == (&val0));
    const auto* val0_const_pt = sample.getAncestor<0>().ancestor;
    EXPECT_TRUE(val0_const_pt == (&val0));

    auto* val1_pt = sample.getAncestor<1>().ancestor;
    EXPECT_TRUE(val1_pt == (&val1));
    const auto* val1_const_pt = sample.getAncestor<1>().ancestor;
    EXPECT_TRUE(val1_const_pt == (&val1));

    auto* val2_pt = sample.getAncestor<2>().ancestor;
    EXPECT_TRUE(val2_pt == (&val2));
    const auto* val2_const_pt = sample.getAncestor<2>().ancestor;
    EXPECT_TRUE(val2_const_pt == (&val2));
}

TEST(AncestorsAware, quadruple_bind_check) {
    ValueStorerTest<int> val0;
    ValueStorerTest<float> val1;
    ValueStorerTest<int> val2;
    ValueStorerTest<std::string> val3;

    AncestorsAwareTest<int, float, int, std::string> sample(val0, val1, val2, val3);

    auto* val0_pt = sample.getAncestor<0>().ancestor;
    EXPECT_TRUE(val0_pt == (&val0));
    const auto* val0_const_pt = sample.getAncestor<0>().ancestor;
    EXPECT_TRUE(val0_const_pt == (&val0));

    auto* val1_pt = sample.getAncestor<1>().ancestor;
    EXPECT_TRUE(val1_pt == (&val1));
    const auto* val1_const_pt = sample.getAncestor<1>().ancestor;
    EXPECT_TRUE(val1_const_pt == (&val1));

    auto* val2_pt = sample.getAncestor<2>().ancestor;
    EXPECT_TRUE(val2_pt == (&val2));
    const auto* val2_const_pt = sample.getAncestor<2>().ancestor;
    EXPECT_TRUE(val2_const_pt == (&val2));

    auto* val3_pt = sample.getAncestor<3>().ancestor;
    EXPECT_TRUE(val3_pt == (&val3));
    const auto* val3_const_pt = sample.getAncestor<3>().ancestor;
    EXPECT_TRUE(val3_const_pt == (&val3));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
