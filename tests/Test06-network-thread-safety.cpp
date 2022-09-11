/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include "ValueExtractor.h"
#include <DynamicFlow/Network.h>

#include <thread>

TEST(Flow, node_creation_while_updating_flow) {
  flw::Flow flow;

  auto source = flow.makeSource<int>("source");
  auto node = flow.makeNode<int, int>(
      [](const int &input) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return input;
      },
      "node", source);

  const int val = 2;

  source.update(val);
  std::atomic_bool spawned = false;
  std::thread th([&]() {
    spawned.store(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    flow.makeNode<int, int>([](const int &input) { return input; }, "node2",
                            node);
  });
  while (!spawned) {
  }
  flow.update();
  th.join();

  auto node2 = flow.findNode<int>("node2");

  EXPECT_EQ(flw::ValueExtractor::impl().get(node.getValue()), val);
  EXPECT_EQ(node2.getValue().status(), flw::ValueStatus::UNSET);

  flow.update();
  EXPECT_EQ(flw::ValueExtractor::impl().get(node.getValue()), val);
  EXPECT_EQ(flw::ValueExtractor::impl().get(node2.getValue()), val);
}

namespace {
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

struct UnComparable {
  int val = 0;

  bool operator==(const UnComparable &) const = delete;
};

using SourceTest = flw::SourceHandler<UnComparable>;
using NodeTest = flw::NodeHandler<UnComparable>;

template <std::size_t Layers, std::size_t MilliSecondsWait = 50>
class FlowTest : public ::testing::Test,
                 public flw::detail::HandlerFinder,
                 public flw::detail::HandlerMaker,
                 public flw::detail::Updater {
protected:
  std::vector<SourceTest> sources;
  std::vector<std::vector<NodeTest>> nodes;

public:
  void SetUp() override {
    auto waiter = [](const UnComparable &in1, const UnComparable &in2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(MilliSecondsWait));
      return UnComparable{in1.val + in2.val};
    };

    sources.reserve(Layers + 1);
    for (std::size_t s = 0; s < (Layers + 1); ++s) {
      sources.push_back(this->makeSource<UnComparable>(make_source_name(s)));
    }

    nodes.reserve(Layers);
    // first layer
    nodes.emplace_back();
    nodes.back().reserve(Layers);
    for (std::size_t n = 0; n < Layers; ++n) {
      nodes.back().push_back(
          this->makeNode<UnComparable, UnComparable, UnComparable>(
              waiter, make_node_name(0, n), sources[n], sources.back()));
    }
    // other layers
    for (std::size_t l = 1; l < Layers; ++l) {
      std::vector<NodeTest> &prev_layer = nodes.back();
      nodes.emplace_back();
      nodes.back().reserve(Layers - l);
      for (std::size_t n = 0; n < Layers - l; ++n) {
        nodes.back().push_back(
            this->makeNode<UnComparable, UnComparable, UnComparable>(
                waiter, make_node_name(l, n), prev_layer[n],
                prev_layer.back()));
      }
    }
  }

  void checkValues() {
    int expected_value = 2;
    for (std::size_t l = 0; l < Layers; ++l) {
      for (std::size_t n = 0; n < Layers - l; ++n) {
        auto node = this->findNode<UnComparable>(make_node_name(l, n));
        EXPECT_EQ(flw::ValueExtractor::impl().get(node.getValue()).val,
                  expected_value);
      }
      expected_value *= 2;
    }
  }

  std::chrono::milliseconds update(std::size_t threads) {
    this->setThreads(threads);
    for (std::size_t s = 0; s < (Layers + 1); ++s) {
      auto to_update = this->findSource<UnComparable>(make_source_name(s));
      to_update.update(1);
    }
    auto tic = std::chrono::high_resolution_clock::now();
    this->flw::detail::Updater::update();
    std::chrono::milliseconds elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - tic);
    this->checkValues();
    return elapsed;
  }

  void executeComparison(std::size_t threads) {
    auto serial_time = this->update(1).count();
    std::cout << "Serial version: " << serial_time << " [ms]" << std::endl;
    auto parallel_time = this->update(threads).count();
    std::cout << "Parallel version with " << threads
              << " threads: " << parallel_time << " [ms]" << std::endl;
    EXPECT_GE(serial_time, parallel_time);
  };
};

} // namespace

using FlowTest5 = FlowTest<5>;
TEST_F(FlowTest5, layers_5) { executeComparison(2); }

using FlowTest7 = FlowTest<7>;
TEST_F(FlowTest7, layers_7) { executeComparison(2); }

using FlowTest10 = FlowTest<10>;
TEST_F(FlowTest10, layers_10) { executeComparison(2); }
