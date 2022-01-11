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
  /**
   * @brief Update the specified source, using the passed value.
   * ATTENTION!!! After calling this method, the Source object is actually
   * udpated, but not the entire flow (i.e. all the nodes depending on this
   * source). You can trigger the flow update by using the methods inside the
   * @UpdaterFlow class.
   * @input the name of the source to update
   * @input the value to use for updating the source
   * @throw In case a source with that name does not exist in this flow
   * @throw In case a source with that name exists, but has a different type
   * other than <T>
   */
  template <typename T>
  void updateSource(const std::string &source_name,
                    std::unique_ptr<T> new_value) {
    std::scoped_lock<std::mutex> creationLock(entityCreationMtx);
    std::scoped_lock<std::mutex> updateLock(updateValuesMtx);
    updateSource_(source_name, std::move(new_value));
    expandRequiringUpdate();
  };

  /**
   * @brief Variadic version of this->updateSource
   */
  template <typename... UpdateInputs>
  void updateSources(UpdateInputs &&...inputs) {
    std::scoped_lock<std::mutex> creationLock(entityCreationMtx);
    std::scoped_lock<std::mutex> updateLock(updateValuesMtx);
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
