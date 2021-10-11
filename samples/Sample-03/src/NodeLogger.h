#pragma once

#include <flow/Flow.h>
#include <flow/NodeHandler.hpp>
#include <list>

class NodeLogger {
public:
  NodeLogger(flw::Flow &flow, const std::string nodeName);

  void update();

private:
  flw::NodeHandler<int> subjectToLog;
  const std::string logName;

  std::list<std::string> rotatingContent;
};
