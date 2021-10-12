/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityAware.hpp>
#include <flow/Updater.h>

namespace flw {

class UpdaterSources : virtual public EntityAware,
                       virtual public Updater,
                       private SourceHandlerResetter,
                       private ValueStorerExtractor {
public:
  // documentare bene che flow non sara aggiornato subito dopo aver aggiornato
  // sources, ma serve chiamare updateFlow()
  template <typename T>
  void updateSource(const std::string &source_name,
                    std::unique_ptr<T> new_value) {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    std::lock_guard<std::mutex> updateLock(updateValuesMtx);
    updateSource_(source_name, std::move(new_value));
    expandRequiringUpdate();
  };

  template <typename... UpdateInputs>
  void updateSources(UpdateInputs &&...inputs) {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    std::lock_guard<std::mutex> updateLock(updateValuesMtx);
    updateSource_(std::forward<UpdateInputs>(inputs)...);
    expandRequiringUpdate();
  }

protected:
  template <typename T, typename... UpdateInputs>
  void updateSource_(const std::string &source_name,
                     std::unique_ptr<T> new_value,
                     UpdateInputs &&...remaining) {
    this->template updateSource_<T>(source_name, std::move(new_value));
    updateSource_(std::forward<UpdateInputs>(remaining)...);
  }
  template <typename T>
  void updateSource_(const std::string &source_name,
                     std::unique_ptr<T> new_value) {
    SourceHandler<T> handler = this->template findSource_<T>(source_name);
    this->reset(std::move(new_value), handler);
    Source<T> *impl = dynamic_cast<Source<T> *>(&extractStorer(handler));
    for (auto *d : impl->descendants) {
      requiringUpdate.emplace(d);
    }
  }

  void expandRequiringUpdate();
};

} // namespace flw
