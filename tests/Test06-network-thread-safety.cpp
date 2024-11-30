/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <gtest/gtest.h>

#include <DynamicFlow/Network.hxx>

#include <algorithm>
#include <thread>

TEST(ThreadSafetyTest, node_creation_while_updating_flow) {
  flw::Flow flow;
  flow.setOnNewNodePolicy(flw::HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE);

  auto source = flow.makeSource<int>(0);
  auto node = flow.makeNode<int, int>(
      [](int input) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return input;
      },
      source);

  bool node2_was_updated = false;

  {
    SCOPED_TRACE("Adding an extra node while updating the flow");

    std::atomic_bool spawned = false;
    std::jthread th([&]() {
      spawned.store(true, std::memory_order_acquire);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      flow.makeNode<int, int>(
          [](int input) { return input; }, node, "",
          [&node2_was_updated](int) { node2_was_updated = true; });
    });
    while (!spawned.load(std::memory_order_acquire)) {
    }
    flow.update();
  }

  ASSERT_FALSE(node2_was_updated);

  flow.update();
  ASSERT_TRUE(node2_was_updated);
}

namespace flw::test {
struct UnComparable {
  int val = 0;

  bool operator==(const UnComparable &) const = delete;
};

class MultiThreadedUpdateFixture : public ::testing::TestWithParam<std::size_t>,
                                   public flw::HandlerFinder,
                                   public flw::HandlerMaker,
                                   public flw::Updater {
protected:
  using Source = HandlerSource<UnComparable>;
  std::vector<Source> sources;
  std::vector<std::vector<int>> nodes_values;

public:
  static constexpr std::size_t THREADS =
      2; // cannot be done with more than 2 threads on the CI but can be changed
         // to do local tests

  template <typename HandlerT>
  void makeNextLevel(std::vector<Handler<UnComparable>> &res,
                     const std::vector<HandlerT> &prev_layer) {
    std::size_t index_a = nodes_values.size();
    nodes_values.emplace_back().resize(prev_layer.size() - 1);
    res.clear();
    res.reserve(prev_layer.size() - 1);
    for (std::size_t index_b = 0; index_b < prev_layer.size() - 1; ++index_b) {
      res.emplace_back(this->makeNode<UnComparable, UnComparable, UnComparable>(
          [](const UnComparable &in1, const UnComparable &in2) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return UnComparable{in1.val + in2.val};
          },
          prev_layer[index_b], prev_layer.back(), "",
          [a = index_a, b = index_b, this](const UnComparable &v) {
            nodes_values[a][b] = v.val;
          }));
    }
  }

  void SetUp() override {
    std::size_t layers = GetParam();
    this->setOnNewNodePolicy(OnNewNodePolicy::DEFERRED_UPDATE);

    for (std::size_t s = 0; s < (layers + 1); ++s) {
      sources.push_back(this->makeSource<UnComparable>(UnComparable{1}));
    }

    // first layer
    std::vector<Handler<UnComparable>> layer;
    makeNextLevel(layer, sources);
    // other layers
    std::vector<Handler<UnComparable>> next;
    for (std::size_t l = 1; l < layers; ++l) {
      makeNextLevel(next, layer);
      std::swap(next, layer);
    }
  }

  bool checkValues() const {
    int expected_val = 2;
    return std::all_of(nodes_values.begin(), nodes_values.end(),
                       [&](const std::vector<int> &layer) {
                         bool res = std::all_of(
                             layer.begin(), layer.end(),
                             [&](int val) { return val == expected_val; });
                         expected_val *= 2;
                         return res;
                       });
  }

  std::chrono::nanoseconds update(std::size_t threads) {
    this->setThreads(threads);

    for (auto &src : sources) {
      src.update(UnComparable{1});
    }

    auto tic = std::chrono::high_resolution_clock::now();
    this->flw::Updater::update();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - tic);
  }
};

TEST_P(MultiThreadedUpdateFixture, compare_durations) {
  auto serial_time = this->update(1);
  ASSERT_TRUE(checkValues());
  std::cout << "Serial version: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   serial_time)
                   .count()
            << " [ms]" << std::endl;

  auto parallel_time = this->update(THREADS);
  ASSERT_TRUE(checkValues());
  std::cout << "Multithreaded version: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   parallel_time)
                   .count()
            << " [ms]" << std::endl;

  EXPECT_GE(serial_time, parallel_time);
}

INSTANTIATE_TEST_CASE_P(MultiThreadedUpdateTests, MultiThreadedUpdateFixture,
                        ::testing::Values(5, 7, 10));
} // namespace flw::test
