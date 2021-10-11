#include <NodeLogger.h>
#include <thread>

int main() {
  flw::Flow flow;

  auto sampleA = flow.makeSource<int>("SampleA");
  auto sampleB = flow.makeSource<int>("SampleB");
  flow.makeNode<int, int, int>(
      "Sum",
      std::function<int(const int &, const int &)>(
          [](const int &a, const int &b) { return a + b; }),
      sampleA, sampleB);
  flow.makeNode<int, int, int>(
      "Diff",
      std::function<int(const int &, const int &)>(
          [](const int &a, const int &b) { return a - b; }),
      sampleA, sampleB);

  std::thread updatingThread([&]() {
    while () {
      /* code */
    }
  });

  return EXIT_SUCCESS;
}
