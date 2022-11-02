#pragma once

#include <DynamicFlow/Network.h>

#include <list>

namespace flw::sample {
class NodeLogger {
public:
  NodeLogger(const flw::NodeHandler<int> &subjectToLog);

  void update();

private:
  flw::NodeHandler<int> subjectToLog;
  std::atomic<int> subjectLastValue = 0;
  const std::string logName;

  std::list<std::string> rotating_content;
};
} // namespace flw::sample
