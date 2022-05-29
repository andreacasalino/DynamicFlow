#pragma once

#include <DynamicFlow/flow/Flow.h>
#include <DynamicFlow/flow/NodeHandler.hpp>
#include <list>

class NodeLogger {
public:
  NodeLogger(const flw::NodeHandler<int> &subjectToLog);

  void update();

private:
  flw::NodeHandler<int> subjectToLog;
  const std::string logName;

  std::list<std::string> rotatingContent;
};
