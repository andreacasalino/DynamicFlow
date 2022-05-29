/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "ValueStorerTest.hpp"
#include <DynamicFlow/components/Evaluator.hpp>
#include <gtest/gtest.h>
#include <string>

template <typename T, typename... Ts>
class EvaluatorTest : public flw::Evaluator<T, Ts...> {
public:
  template <typename... Values>
  EvaluatorTest(const std::function<T(const Ts &...)> &evaluation,
                const Values &...ancestors)
      : flw::Evaluator<T, Ts...>(evaluation) {
    flw::AncestorsAware<Ts...>::bind(ancestors...);
  };
};

TEST(Evaluator, evaluation_not_ready_expected) {
  ValueStorerTest<int> val0;
  ValueStorerTest<float> val1;
  ValueStorerTest<int> val2;

  bool something_was_done = false;
  EvaluatorTest<std::string, int, float, int> sample(
      [&something_was_done](const int &in0, const float &in1, const int &in2) {
        something_was_done = true;
        return "done";
      },
      val0, val1, val2);

  EXPECT_FALSE(something_was_done);
  EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);
  EXPECT_FALSE(sample.value.isValue());

  val0.reset(5);
  EXPECT_FALSE(something_was_done);
  EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);
  EXPECT_FALSE(sample.value.isValue());

  val1.reset(3.5f);
  EXPECT_FALSE(something_was_done);
  EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::NOT_READY);
  EXPECT_FALSE(sample.value.isValue());
}

TEST(Evaluator, evaluation_success_expected) {
  ValueStorerTest<int> val0;
  ValueStorerTest<float> val1;
  ValueStorerTest<int> val2;

  val0.reset(5);
  val1.reset(3.5f);
  val2.reset(5);

  const std::string expected_result = "done";
  bool something_was_done = false;
  EvaluatorTest<std::string, int, float, int> sample(
      [&something_was_done, &expected_result](const int &in0, const float &in1,
                                              const int &in2) {
        something_was_done = true;
        return expected_result;
      },
      val0, val1, val2);

  EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::SUCCESS);
  EXPECT_TRUE(something_was_done);
  EXPECT_TRUE(sample.value.isValue());
  EXPECT_EQ(*sample.value.get(), expected_result);
}

class Dummy {
public:
  Dummy() { throw std::runtime_error("Dummy exception"); }
};

TEST(Evaluator, evaluation_exception_blocking_expected) {
  ValueStorerTest<int> val0;
  ValueStorerTest<Dummy> val1;
  ValueStorerTest<int> val2;

  val0.reset(5);
  val1.reset();
  val2.reset(5);

  bool something_was_done = false;
  EvaluatorTest<std::string, int, Dummy, int> sample(
      [&something_was_done](const int &in0, const Dummy &in1, const int &in2) {
        something_was_done = true;
        return "done";
      },
      val0, val1, val2);

  EXPECT_EQ(sample.evaluate(), flw::EvaluationResult::BLOCKING_EXCEPTION);
  EXPECT_FALSE(something_was_done);
  EXPECT_FALSE(sample.value.isValue());
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
