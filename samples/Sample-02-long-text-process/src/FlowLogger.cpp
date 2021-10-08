#include <FlowLogger.h>
#include <sstream>

std::size_t FlowLogger::counter = 0;

void FlowLogger::log(flw::PrintCapable &flow) {
  ++counter;

  std::stringstream logName;
  logName << "Log-" << counter;

  flw::log(logName.str(), flow);
}