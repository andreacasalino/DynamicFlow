/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/UpdaterSources.h>

namespace flw {

void UpdaterSources::expandRequiringUpdate() {
  std::set<EvaluateCapable *> open = requiringUpdate;
  std::set<EvaluateCapable *> close;
  while (!open.empty()) {
    auto *front = *open.begin();
    open.erase(open.begin());
    auto *impl = dynamic_cast<DescendantsAware *>(front);
    for (auto *d : impl->descendants) {
      open.emplace(d);
    }
    close.emplace(front);
  }
  requiringUpdate = std::move(close);
}

} // namespace flw