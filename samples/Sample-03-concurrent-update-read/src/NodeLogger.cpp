#include <NodeLogger.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

namespace flw::sample {
NodeLogger::NodeLogger(const flw::NodeHandler<int> &subjectToLog)
    : subjectToLog(subjectToLog),
      logName(subjectToLog.label().value() + std::string(".txt")) {
  {
    std::ofstream log(
        logName); // just to clear the file content if already exists
  }
  this->subjectToLog.onNewValueCallBack(
      [&recipient = this->subjectLastValue](const int &v) {
        recipient.store(v);
      });
  std::cout << "check content of: " << logName << " after the process stops"
            << std::endl;
}

namespace {
std::string getTimeStr() {
  std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  std::string s(50, '\0');
  std::size_t size = std::strftime(s.data(), s.size(), "%Y-%m-%d %H:%M:%S",
                                   std::localtime(&now));
  return std::string(s.data());
}
} // namespace

void NodeLogger::update() {
  std::stringstream newLine;
  {
    newLine << getTimeStr();
    newLine << " generation: " << subjectToLog.getValue().epoch() << " value: ";
    switch (subjectToLog.getValue().status()) {
    case flw::ValueStatus::VALUE:
      newLine << subjectLastValue;
      break;
    case flw::ValueStatus::EXCEPTION:
      newLine << "EXCEPTION";
      break;
    case flw::ValueStatus::UNSET:
      newLine << "UNSET";
      break;
    }
  }

  if (200 == rotating_content.size()) {
    rotating_content.pop_front();
  }
  rotating_content.push_back(newLine.str());

  std::ofstream log(logName);
  for (const auto line : rotating_content) {
    log << line << std::endl;
  }
}
} // namespace flw::sample
