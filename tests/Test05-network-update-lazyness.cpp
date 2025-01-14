// /**
//  * Author:    Andrea Casalino
//  * Created:   10.09.2021
//  *
//  * report any bug to andrecasa91@gmail.com.
//  **/

#include <gtest/gtest.h>

#include <DynamicFlow/Network.hxx>

struct Summer {
  int operator()(int val) { return val; }

  int operator()(int a, int b) { return a + b; }
};

class LazynessTest : public ::testing::Test,
                     public flw::HandlerMaker,
                     public flw::Updater {
public:
  LazynessTest() = default;

  struct Source {
    Source(flw::HandlerSource<int> &&s)
        : source{std::forward<flw::HandlerSource<int>>(s)} {}

    int val = 0;
    flw::HandlerSource<int> source;
  };
  std::vector<Source> sources;

  struct NodeValue {
    int val;
    std::size_t epochs = 0;
  };
  NodeValue M0_val;
  NodeValue M1_val;
  NodeValue D0_val;
  NodeValue D1_val;
  NodeValue F0_val;

  template <typename... As>
  auto makeNode_(NodeValue &rec, const flw::Handler<As> &...deps) {
    return this->makeNode<int, As...>(
        [](const As &...vals) {
          static Summer s;
          return s(vals...);
        },
        deps..., "",
        [&rec](int val) {
          rec.val = val;
          ++rec.epochs;
        });
  }

  template <std::size_t Index> void updateSource(int val) {
    sources[Index].val = val;
    sources[Index].source.update(val);
  }

  void SetUp() override {
    auto cloner = [](const auto &in) { return in; };
    auto combiner = [](const auto &in1, const auto &in2) { return in1 + in2; };

    sources.emplace_back(this->makeSource<int>(0));
    sources.emplace_back(this->makeSource<int>(0));
    sources.emplace_back(this->makeSource<int>(0));
    sources.emplace_back(this->makeSource<int>(0));

    auto M0 =
        this->makeNode_<int, int>(M0_val, sources[0].source, sources[1].source);
    auto M1 =
        this->makeNode_<int, int>(M1_val, sources[2].source, sources[3].source);

    auto D0 = this->makeNode_<int>(D0_val, M0);
    auto D1 = this->makeNode_<int, int>(D1_val, M0, M1);

    auto F0 = this->makeNode_<int, int>(F0_val, D0, D1);

    this->checkValues();
  }

  void checkValues() {
    EXPECT_EQ(sources[0].val + sources[1].val, M0_val.val);
    EXPECT_EQ(sources[2].val + sources[3].val, M1_val.val);

    EXPECT_EQ(M0_val.val, D0_val.val);
    EXPECT_EQ(M0_val.val + M1_val.val, D1_val.val);

    EXPECT_EQ(D0_val.val + D1_val.val, F0_val.val);
  }
};

TEST_F(LazynessTest, case_0_same_source_values) {
  this->updateSource<0>(0);
  this->updateSource<1>(0);
  this->updateSource<2>(0);
  this->updateSource<3>(0);

  this->update();

  // no update expected as the value is still the same
  EXPECT_EQ(M0_val.epochs, 1);
  EXPECT_EQ(M1_val.epochs, 1);
  EXPECT_EQ(D0_val.epochs, 1);
  EXPECT_EQ(D1_val.epochs, 1);
  EXPECT_EQ(F0_val.epochs, 1);
}

TEST_F(LazynessTest, case_1_0_only_one_of_the_source) {
  this->updateSource<0>(1);

  this->update();

  this->checkValues();

  EXPECT_EQ(M0_val.val, 1);
  EXPECT_EQ(M0_val.epochs, 2);
  EXPECT_EQ(M1_val.val, 0);
  EXPECT_EQ(M1_val.epochs, 1);

  EXPECT_EQ(D0_val.val, 1);
  EXPECT_EQ(D0_val.epochs, 2);
  EXPECT_EQ(D1_val.val, 1);
  EXPECT_EQ(D1_val.epochs, 2);

  EXPECT_EQ(F0_val.val, 2);
  EXPECT_EQ(F0_val.epochs, 2);
}

TEST_F(LazynessTest, case_1_1_only_one_of_the_source) {
  this->updateSource<2>(1);

  this->update();

  this->checkValues();

  EXPECT_EQ(M0_val.val, 0);
  EXPECT_EQ(M0_val.epochs, 1);
  EXPECT_EQ(M1_val.val, 1);
  EXPECT_EQ(M1_val.epochs, 2);

  EXPECT_EQ(D0_val.val, 0);
  EXPECT_EQ(D0_val.epochs, 1);
  EXPECT_EQ(D1_val.val, 1);
  EXPECT_EQ(D1_val.epochs, 2);

  EXPECT_EQ(F0_val.val, 1);
  EXPECT_EQ(F0_val.epochs, 2);
}

TEST_F(LazynessTest, case_2_multiple_sources) {
  this->updateSource<0>(1);
  this->updateSource<2>(2);

  this->update();
  this->checkValues();

  EXPECT_EQ(M0_val.val, 1);
  EXPECT_EQ(M0_val.epochs, 2);
  EXPECT_EQ(M1_val.val, 2);
  EXPECT_EQ(M1_val.epochs, 2);

  EXPECT_EQ(D0_val.val, 1);
  EXPECT_EQ(D0_val.epochs, 2);
  EXPECT_EQ(D1_val.val, 3);
  EXPECT_EQ(D1_val.epochs, 2);

  EXPECT_EQ(F0_val.val, 4);
  EXPECT_EQ(F0_val.epochs, 2);
}
