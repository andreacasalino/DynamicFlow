/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/flow/EntityAware.hpp>
#include <DynamicFlow/flow/Flow.h>
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

/**
 * @throw In case the passed fileName is an invalid file path for creating the
 * log
 */
void log(const std::string &fileName, const flw::PrintCapable &subject);
} // namespace flw

std::ostream &operator<<(std::ostream &stream,
                         const flw::PrintCapable &subject);
