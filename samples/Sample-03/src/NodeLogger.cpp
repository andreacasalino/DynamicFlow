#include <NodeLogger.h>
#include <chrono>
#include <fstream>
#include <sstream>

NodeLogger::NodeLogger(flw::Flow &flow, const std::string nodeName)
    : subjectToLog(flow.findNode<int>(nodeName)), logName(nodeName) {}

void NodeLogger::update() {
  auto value = flw::copyValue(subjectToLog);
  // todo log time
  throw 0;
  std::stringstream newLine;

  if (20 == rotatingContent.size()) {
    rotatingContent.pop_front();
  }
  rotatingContent.push_back(newLine.str());

  std::ofstream log(logName);
  for (const auto line : rotatingContent) {
    log << line << std::endl;
  }
}
