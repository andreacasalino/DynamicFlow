#include <DynamicFlow/Node.hxx>

namespace flw {
bool Resettable::updatePossible() const {
  return std::all_of(
      dependencies_.begin(), dependencies_.end(),
      [](const FlowElementResettablePtr &ptr) { return ptr->hasValue(); });
}
} // namespace flw
