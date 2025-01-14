#include <DynamicFlow/Network.hxx>

#include <RunScript.h>
#include <ValueListener.h>

#include <iostream>
#include <random>
#include <filesystem>
#include <atomic>
#include <signal.h>

std::atomic_bool life = true;

void handle(int sig) {
  life.store(false, std::memory_order_release);
}

int main() {
  signal(SIGINT, handle);
  signal(SIGABRT, handle);
  signal(SIGSEGV, handle);
  signal(SIGTERM, handle);

  flw::Flow flow;

  std::cout << "Logging sum values at: " << flw::sample::LogDir::get() / "sum.log" << std::endl;
  flw::sample::ValueListener sum_listener{flw::sample::LogDir::get() / "sum.log", std::chrono::milliseconds(50)};

  std::cout << "Logging diff values at: " << flw::sample::LogDir::get() / "diff.log" << std::endl;
  flw::sample::ValueListener diff_listener{flw::sample::LogDir::get() / "diff.log", std::chrono::milliseconds(50)};

  // build a simple flow with 2 int as sources and 2 nodes representing their
  // sum and difference
  auto sampleA = flow.makeSource<int>(0, "SampleA");
  auto sampleB = flow.makeSource<int>(0, "SampleB");
  auto sum_node = flow.makeNode<int, int, int>(
      [](int a, int b) { 
        return a + b; 
      }, sampleA, sampleB, "", [&sum_listener](int val) {
        sum_listener.onNewValue(val);
      });
  auto diff_node = flow.makeNode<int, int, int>(
      [](int a, int b) {
        return a - b;
      }, sampleA, sampleB, "", [&diff_listener](int val) {
        diff_listener.onNewValue(val);
      });

  std::vector<std::jthread> threads;
  // spawn the thread updating the flow
  threads.emplace_back([&flow](std::stop_token token) {
    while(!token.stop_requested()) {
      flow.update();
      std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }
  });
  // spawn the updater of the first source
  threads.emplace_back([&sampleA](std::stop_token token){
    while(!token.stop_requested()) {
      sampleA.update(rand() % 10 - 5);
    }
  });  
  // spawn the updater of the second source
  threads.emplace_back([&sampleB](std::stop_token token){
    while(!token.stop_requested()) {
      sampleB.update(rand() % 10 - 5);
    }
  });
  // spawn the periodic file dumpers
  threads.emplace_back(sum_listener.startAndTransfer());
  threads.emplace_back(diff_listener.startAndTransfer());

  std::cout << "press Ctrl+c to stop" << std::endl;
  while(life.load(std::memory_order_acquire)) {
  }
  std::cout << "stopping ... " << std::endl;
  threads.clear();

  return EXIT_SUCCESS;
}
