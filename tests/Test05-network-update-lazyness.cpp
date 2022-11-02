// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/

#include <gtest/gtest.h>

#include "CheckCPlusPlus20.h"
#include "ValueExtractor.h"
#include <DynamicFlow/Network.h>

namespace {
class LazynessTest : public ::testing::Test,
                     public flw::detail::HandlerFinder,
                     public flw::detail::HandlerMaker,
                     public flw::detail::Updater {
public:
  LazynessTest() = default;

  void SetUp() override {
    auto cloner = [](const auto &in) { return in; };
    auto combiner = [](const auto &in1, const auto &in2) { return in1 + in2; };

    auto S0 = this->makeSource<int>("S0");
    auto S1 = this->makeSource<int>("S1");
    auto S2 = this->makeSource<int>("S2");
    auto S3 = this->makeSource<int>("S3");

    auto M0 = this->makeNode<int, int, int>(combiner, "M0", S0, S1);

    auto M1 = this->makeNode<int, int, int>(combiner, "M1", S2, S3);

    auto D0 = this->makeNode<int, int>(cloner, "D0", M0);

    auto D1 = this->makeNode<int, int, int>(combiner, "D1", M0, M1);

    auto F0 = this->makeNode<int, int, int>(combiner, "F0", D0, D1);

    S0.update(1);
    S1.update(1);
    S2.update(1);
    S3.update(1);
    this->update();

    this->checkValues();
  }

  void checkValues() {
    auto x =
        flw::ValueExtractor::impl().get(this->findSource<int>("S0").getValue());
    auto y =
        flw::ValueExtractor::impl().get(this->findSource<int>("S1").getValue());
    auto z =
        flw::ValueExtractor::impl().get(this->findSource<int>("S2").getValue());
    auto w =
        flw::ValueExtractor::impl().get(this->findSource<int>("S3").getValue());

    auto M0 = this->findNode<int>("M0");
    EXPECT_EQ(flw::ValueExtractor::impl().get(M0.getValue()), x + y);

    auto M1 = this->findNode<int>("M1");
    EXPECT_EQ(flw::ValueExtractor::impl().get(M1.getValue()), z + w);

    auto D0 = this->findNode<int>("D0");
    EXPECT_EQ(flw::ValueExtractor::impl().get(D0.getValue()), x + y);

    auto D1 = this->findNode<int>("D1");
    EXPECT_EQ(flw::ValueExtractor::impl().get(D1.getValue()), x + y + z + w);

    auto F0 = this->findNode<int>("F0");
    EXPECT_EQ(flw::ValueExtractor::impl().get(F0.getValue()),
              2 * (x + y) + z + w);
  }
};
} // namespace

TEST_F(LazynessTest, case_0) {
  CHECK_CPLUSPLUS_20

  auto S0 = this->findSource<int>("S0");
  S0.update(1);
  auto S1 = this->findSource<int>("S1");
  S1.update(1);

  this->update();
  this->checkValues();

  // nothing should have been changed as the sources values are the same
  auto M0 = this->findNode<int>("M0");
  EXPECT_EQ(M0.getValue().epoch(), 1);

  auto M1 = this->findNode<int>("M1");
  EXPECT_EQ(M1.getValue().epoch(), 1);

  auto D0 = this->findNode<int>("D0");
  EXPECT_EQ(D0.getValue().epoch(), 1);

  auto D1 = this->findNode<int>("D1");
  EXPECT_EQ(D1.getValue().epoch(), 1);

  auto F0 = this->findNode<int>("F0");
  EXPECT_EQ(F0.getValue().epoch(), 1);
}

TEST_F(LazynessTest, case_1) {
  CHECK_CPLUSPLUS_20

  auto S0 = this->findSource<int>("S0");
  S0.update(2);
  auto S1 = this->findSource<int>("S1");
  S1.update(2);

  this->update();
  this->checkValues();

  auto M0 = this->findNode<int>("M0");
  EXPECT_EQ(M0.getValue().epoch(), 2);

  auto M1 = this->findNode<int>("M1");
  EXPECT_EQ(M1.getValue().epoch(), 1);

  auto D0 = this->findNode<int>("D0");
  EXPECT_EQ(D0.getValue().epoch(), 2);

  auto D1 = this->findNode<int>("D1");
  EXPECT_EQ(D1.getValue().epoch(), 2);

  auto F0 = this->findNode<int>("F0");
  EXPECT_EQ(F0.getValue().epoch(), 2);
}

TEST_F(LazynessTest, case_2) {
  CHECK_CPLUSPLUS_20

  auto S2 = this->findSource<int>("S2");
  S2.update(3);
  auto S3 = this->findSource<int>("S3");
  S3.update(3);

  this->update();
  this->checkValues();

  auto M0 = this->findNode<int>("M0");
  EXPECT_EQ(M0.getValue().epoch(), 1);

  auto M1 = this->findNode<int>("M1");
  EXPECT_EQ(M1.getValue().epoch(), 2);

  auto D0 = this->findNode<int>("D0");
  EXPECT_EQ(D0.getValue().epoch(), 1);

  auto D1 = this->findNode<int>("D1");
  EXPECT_EQ(D1.getValue().epoch(), 2);

  auto F0 = this->findNode<int>("F0");
  EXPECT_EQ(F0.getValue().epoch(), 2);
}
