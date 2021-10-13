/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Flow.h>

namespace flw {
Flow::Flow(Flow &&o) { this->operator=(std::move(o)); };

Flow &Flow::operator=(Flow &&o) {
  std::lock_guard<std::mutex> lockCreation(entityCreationMtx);
  std::lock_guard<std::mutex> lockUpdate(updateValuesMtx);

  this->sources = std::move(o.sources);
  this->nodes = std::move(o.nodes);
  this->allTogether = std::move(o.allTogether);
  this->requiringUpdate = std::move(o.requiringUpdate);

  o.sources.clear();
  o.nodes.clear();
  o.allTogether.clear();
  o.requiringUpdate.clear();
}
} // namespace flw
