/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include "ValueAwareTest.hpp"
#include <Evaluator.hpp>
#include <string>

template<typename ... Ts>
class EvaluatorTest
    : public flw::Evaluator<Ts...> {
public:
    template<typename ... Values>
    EvaluatorTest(const std::function<void(const Ts & ...)>& evaluation, const Values& ... ancestors)
        : flw::Evaluator<Ts...>(evaluation) {
        bind(ancestors...);
    };
};

TEST(Evaluator, evaluation_not_ready_expected) {
    ValueAwareTest<int> val0;
    ValueAwareTest<float> val1;
    ValueAwareTest<int> val2;

    bool something_was_done = false;
    EvaluatorTest<int, float, int> sample([&something_was_done](const int& in0, const float& in1, const int& in2) {
        something_was_done = true;
        }, val0, val1, val2);

    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);

    val0.value.reset(5);
    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);

    val1.value.reset(3.5f);
    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);
}

TEST(Evaluator, evaluation_success_expected) {
    ValueAwareTest<int> val0;
    ValueAwareTest<float> val1;
    ValueAwareTest<int> val2;

    val0.value.reset(5);
    val1.value.reset(3.5f);
    val2.value.reset(5);

    bool something_was_done = false;
    EvaluatorTest<int, float, int> sample([&something_was_done](const int& in0, const float& in1, const int& in2) {
        something_was_done = true;
        }, val0, val1, val2);

    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::SUCCESS);
    EXPECT_TRUE(something_was_done);
}

class Dummy {
public:
    Dummy(const int& val) {
        throw std::runtime_error("Dummy exception");
    }
};

TEST(Evaluator, evaluation_exception_expected) {
    ValueAwareTest<int> val0;
    ValueAwareTest<Dummy> val1;
    ValueAwareTest<int> val2;

    val0.value.reset(5);
    val1.value.reset(1);
    val2.value.reset(5);

    bool something_was_done = false;
    EvaluatorTest<int, Dummy, int> sample([&something_was_done](const int& in0, const Dummy& in1, const int& in2) {
        something_was_done = true;
        }, val0, val1, val2);

    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::EXCEPTION);
    EXPECT_FALSE(something_was_done);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
