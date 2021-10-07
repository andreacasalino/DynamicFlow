/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>
#include <gtest/gtest.h>
#include <sstream>
#include <thread>

std::string make_source_name(const std::size_t position) {
  std::stringstream stream;
  stream << 'S' << position;
  return stream.str();
}

std::string make_node_name(const std::size_t layer,
                           const std::size_t position) {
  std::stringstream stream;
  stream << "L-" << layer << '-' << position;
  return stream.str();
}

using SourceTest = flw::SourceHandler<int>;
using NodeTest = flw::NodeHandler<int, int, int>;

template <std::size_t Layers, std::size_t MilliSecondsWait = 500>
class FlowTest : public ::testing::Test, public flw::Flow {
protected:
  std::vector<SourceTest> sources;
  std::vector<std::vector<NodeTest>> nodes;

public:
  void SetUp() override {
    const std::function<int(const int &, const int &)> waiter =
        [](const auto &in1, const auto &in2) {
          std::this_thread::sleep_for(
              std::chrono::milliseconds(MilliSecondsWait));
          return in1 + in2;
        };

    sources.reserve(Layers + 1);
    for (std::size_t s = 0; s < (Layers + 1); ++s) {
      sources.push_back(this->makeSource<int>(make_source_name(s)));
    }

    nodes.reserve(Layers);
    // first layer
    nodes.emplace_back();
    nodes.back().reserve(Layers);
    for (std::size_t n = 0; n < Layers; ++n) {
      nodes.back().push_back(this->makeNode(make_node_name(0, n), waiter,
                                            sources[n], sources.back()));
    }
    // other layers
    for (std::size_t l = 1; l < Layers; ++l) {
      std::vector<NodeTest> &prev_layer = nodes.back();
      nodes.emplace_back();
      nodes.back().reserve(Layers - l);
      for (std::size_t n = 0; n < Layers - l; ++n) {
        nodes.back().push_back(this->makeNode(
            make_node_name(l, n), waiter, prev_layer[n], prev_layer.back()));
      }
    }
  }

  std::chrono::milliseconds update(std::size_t threads) {
    this->setThreadsForUpdate(threads);
    for (std::size_t s = 0; s < (Layers + 1); ++s) {
      this->template updateSource<int>(make_source_name(s),
                                       std::make_unique<int>(0));
    }
    auto tic = std::chrono::high_resolution_clock::now();
    this->updateFlow();
    std::chrono::milliseconds elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - tic);
    this->checkValues();
    return elapsed;
  }

  void checkValues() {
    for (std::size_t l = 0; l < Layers; ++l) {
      for (std::size_t n = 0; n < Layers - l; ++n) {
        auto node = this->findNode<int, int, int>(make_node_name(l, n));
        EXPECT_TRUE(node.isValue());
        EXPECT_EQ(flw::copyValue(node), l + 1);
      }
    }
  }
};

 using FlowTest3 = FlowTest<3>;
 TEST_F(FlowTest3, layers_size_3) { 
     update(1);
    //EXPECT_GE(update(1), update(4)); 
 }

// using FlowTest5 = FlowTest<5>;
// TEST_F(FlowTest5, layers_size_5) { EXPECT_GE(update(1), update(4)); }

// using FlowTest7 = FlowTest<7>;
// TEST_F(FlowTest7, layers_size_7) { EXPECT_GE(update(1), update(4)); }

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
