#include "ValueExtractor.h"

namespace flw {
ValueExtractor &ValueExtractor::impl() {
  static ValueExtractor singleton = ValueExtractor{};
  return singleton;
}
} // namespace flw
