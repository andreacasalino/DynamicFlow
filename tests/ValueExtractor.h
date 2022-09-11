#pragma once

#include <DynamicFlow/Network.h>

namespace flw {
class ValueExtractor : public detail::CanGetValue {
public:
  static ValueExtractor &impl();

  using detail::CanGetValue::get;

private:
  ValueExtractor() = default;
};
} // namespace flw
