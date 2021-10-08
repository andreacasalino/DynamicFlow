/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityAware.hpp>
#include <flow/Flow.h>
#include <ostream>

namespace flw {
class PrintCapable : virtual public EntityAware {
public:
  virtual void print(std::ostream &stream) const = 0;
};

class PrintBasic : public PrintCapable {
public:
  void print(std::ostream &stream) const override;
};

template <typename FlowT = Flow, typename PrintableT = PrintBasic>
class PrintableFlow : public FlowT, public PrintableT {
public:
  PrintableFlow() = default;
};

void log(const std::string &fileName, flw::PrintCapable &subject);
} // namespace flw

std::ostream &operator<<(std::ostream &stream, flw::PrintCapable &subject);
