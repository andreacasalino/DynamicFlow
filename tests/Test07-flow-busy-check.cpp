/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <DynamicFlow/flow/Flow.h>
#include <gtest/gtest.h>
#include <thread>

TEST(Flow, generation_update) {
  flw::Flow flow;

  auto source = flow.makeSource<int>("source");
  auto node = flow.makeNode(
      "node",
      std::function<int(const int &)>([](const int &input) { return input; }),
      source);

  EXPECT_EQ(source.getGeneration(), 0);
  for (std::size_t k = 0; k < 10; ++k) {
    flow.updateSourcesAndFlow(source.getName(),
                              std::make_unique<int>(static_cast<int>(k)));
    EXPECT_EQ(source.getGeneration(), k + 1);
    EXPECT_EQ(node.getGeneration(), k + 1);
  }
}

TEST(Flow, busy_check) {
  flw::Flow flow;

  std::atomic_bool updateStarted = false;
  auto source = flow.makeSource<int>("source");
  auto node = flow.makeNode(
      "node",
      std::function<int(const int &)>([&updateStarted](const int &input) {
        updateStarted = true;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return input;
      }),
      source);

  EXPECT_FALSE(flow.isUpdatingFlow());

  std::thread th([&]() {
    while (!updateStarted) {
    }
    EXPECT_TRUE(flow.isUpdatingFlow());
  });

  flow.updateSourcesAndFlow(source.getName(), std::make_unique<int>(0));
  th.join();
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
