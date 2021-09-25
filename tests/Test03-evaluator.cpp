/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>
#include "ValueStorerTest.hpp"
#include <components/Evaluator.hpp>
#include <string>

template<typename T, typename ... Ts>
class EvaluatorTest
    : public flw::Evaluator<T, Ts...> {
public:
    template<typename ... Values>
    EvaluatorTest(const std::function<T(const Ts & ...)>& evaluation, const Values& ... ancestors)
        : flw::Evaluator<T, Ts...>(evaluation) {
        bind(ancestors...);
    };
};

TEST(Evaluator, evaluation_not_ready_expected) {
    ValueStorerTest<int> val0;
    ValueStorerTest<float> val1;
    ValueStorerTest<int> val2;

    bool something_was_done = false;
    EvaluatorTest<std::string, int, float, int> sample([&something_was_done](const int& in0, const float& in1, const int& in2) {
        something_was_done = true;
        return "done";
        }, val0, val1, val2);

    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);

    val0.reset(5);
    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);

    val1.reset(3.5f);
    EXPECT_FALSE(something_was_done);
    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);
}

TEST(Evaluator, evaluation_success_expected) {
    ValueStorerTest<int> val0;
    ValueStorerTest<float> val1;
    ValueStorerTest<int> val2;

    val0.reset(5);
    val1.reset(3.5f);
    val2.reset(5);

    bool something_was_done = false;
    EvaluatorTest<std::string, int, float, int> sample([&something_was_done](const int& in0, const float& in1, const int& in2) {
        something_was_done = true;
        return "done";
        }, val0, val1, val2);

    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::SUCCESS);
    EXPECT_TRUE(something_was_done);
}

class Dummy {
public:
    Dummy() {
        throw std::runtime_error("Dummy exception");
    }
};

TEST(Evaluator, evaluation_exception_blocking_expected) {
    ValueStorerTest<int> val0;
    ValueStorerTest<Dummy> val1;
    ValueStorerTest<int> val2;

    val0.reset(5);
    val1.reset();
    val2.reset(5);

    bool something_was_done = false;
    EvaluatorTest<std::string, int, Dummy, int> sample([&something_was_done](const int& in0, const Dummy& in1, const int& in2) {
        something_was_done = true;
        return "done";
        }, val0, val1, val2);

    EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::BLOCKING_EXCEPTION);
    EXPECT_FALSE(something_was_done);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
