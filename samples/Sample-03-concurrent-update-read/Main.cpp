#include <ActionRepeater.h>
#include <NodeLogger.h>
#include <iostream>
#include <random>

static const std::chrono::microseconds SAMPLE_TIME_FLOW_UPDATE =
    std::chrono::milliseconds(250);

static const std::chrono::microseconds SAMPLE_TIME_LOG =
    std::chrono::milliseconds(50);

int main() {
  flw::Flow flow;

  // build a simple flow with 2 int as sources and 2 nodes representing their
  // sum and difference
  auto sampleA = flow.makeSource<int>("SampleA");
  auto sampleB = flow.makeSource<int>("SampleB");
  auto sum_node = flow.makeNode<int, int, int>(
      [](int a, int b) { return a + b; }, "Sum", sampleA, sampleB);
  auto diff_node = flow.makeNode<int, int, int>(
      [](int a, int b) { return a - b; }, "Diff", sampleA, sampleB);

  std::atomic_bool life = true;

  // spawn the thread updating the flow by sampling everytime new numbers for
  // the sources
  std::thread updatingThread([&]() {
    flw::sample::repeat_action(
        [&]() {
          sampleA.update(rand() % 10 - 5);
          sampleB.update(rand() % 10 - 5);
          flow.update();
        },
        life, SAMPLE_TIME_FLOW_UPDATE);
  });

  // spawn 2 threads logging the content of the nodes with a regular sampling
  // time. During the execution of the program 2 log files named Diff.txt and
  // Sum.txt should appear
  std::atomic<std::size_t> spawned_loggers = 0;
  std::thread sum_node_logger([&]() {
    flw::sample::NodeLogger logger(sum_node);
    ++spawned_loggers;
    flw::sample::repeat_action([&logger]() { logger.update(); }, life,
                               SAMPLE_TIME_LOG);
  });
  std::thread diff_node_logger([&]() {
    flw::sample::NodeLogger logger(diff_node);
    ++spawned_loggers;
    flw::sample::repeat_action([&logger]() { logger.update(); }, life,
                               SAMPLE_TIME_LOG);
  });
  while (spawned_loggers.load() != 2) {
  }

  // this keeps loop until the user press 'k'
  std::cout << "Press k to stop" << std::endl;
  char key_pressed;
  while (life.load()) {
    std::cin >> key_pressed;
    if ('k' == key_pressed) {
      life.store(false);
    }
  }

  updatingThread.join();
  sum_node_logger.join();
  diff_node_logger.join();
  return EXIT_SUCCESS;
}
