#pragma once

#include <flow/Printer.h>

class FlowLogger {
public:
  static void log(flw::PrintCapable &flow);

private:
  static std::size_t counter;
};
