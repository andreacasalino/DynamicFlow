#include <ActionRepeater.h>
#include <NodeLogger.h>
#include <iostream>
#include <random>

static const std::chrono::microseconds SAMPLE_TIME_FLOW_UPDATE =
    std::chrono::milliseconds(20);
static const std::chrono::microseconds SAMPLE_TIME_LOG =
    std::chrono::milliseconds(200);

std::unique_ptr<int> make_sample();

int main() {
  flw::Flow flow;

  auto sampleA = flow.makeSource<int>("SampleA");
  auto sampleB = flow.makeSource<int>("SampleB");
  auto sum_node = flow.makeNode<int, int, int>(
      "Sum",
      std::function<int(const int &, const int &)>(
          [](const int &a, const int &b) { return a + b; }),
      sampleA, sampleB);
  auto diff_node = flow.makeNode<int, int, int>(
      "Diff",
      std::function<int(const int &, const int &)>(
          [](const int &a, const int &b) { return a - b; }),
      sampleA, sampleB);

  std::atomic_bool life = true;

  std::thread updatingThread([&]() {
    repeat_action(
        [&]() {
          int numberA = rand() % 10 - 5;
          int numberB = rand() % 10 - 5;
          flow.updateSourcesAndFlow(sampleA.getName(), make_sample(),
                                    sampleB.getName(), make_sample());
        },
        life, SAMPLE_TIME_FLOW_UPDATE);
  });
  std::thread sum_node_logger([&]() {
    NodeLogger logger(sum_node);
    repeat_action([&logger]() { logger.update(); }, life, SAMPLE_TIME_LOG);
    logger.update();
  });
  std::thread diff_node_logger([&]() {
    NodeLogger logger(diff_node);
    repeat_action([&logger]() { logger.update(); }, life, SAMPLE_TIME_LOG);
    logger.update();
  });

  std::cout << "Press k to stop" << std::endl;
  char key_pressed;
  while (life) {
    std::cin >> key_pressed;
    if ('k' == key_pressed) {
      life = false;
    }
  }

  updatingThread.join();
  sum_node_logger.join();
  diff_node_logger.join();
  return EXIT_SUCCESS;
}

std::unique_ptr<int> make_sample() {
  return std::make_unique<int>(rand() & 10 - 5);
}
