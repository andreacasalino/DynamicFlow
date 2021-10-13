#include <NodeLogger.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <ctime>

NodeLogger::NodeLogger(const flw::NodeHandler<int> &subjectToLog)
    : subjectToLog(subjectToLog),
      logName(subjectToLog.getName() + std::string(".txt")) {}

std::string getTimeStr() {
  std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  std::string s(50, '\0');
  std::size_t size = std::strftime(s.data(), s.size(), "%Y-%m-%d %H:%M:%S",
                                   std::localtime(&now));
  return std::string(s.data());
}

void NodeLogger::update() {
  std::stringstream newLine;
  newLine << getTimeStr();
  newLine << " generation: " << subjectToLog.getGeneration() << " value: ";
  if (subjectToLog.isValue()) {
    newLine << flw::copyValue(subjectToLog);
  } else {
    newLine << "NULL";
  }

  if (20 == rotatingContent.size()) {
    rotatingContent.pop_front();
  }
  rotatingContent.push_back(newLine.str());

  std::ofstream log(logName);
  for (const auto line : rotatingContent) {
    log << line << std::endl;
  }
}
