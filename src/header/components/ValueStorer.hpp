/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <atomic>
#include <components/ValueOrException.hpp>
#include <mutex>

namespace flw {

template <typename T> class ValueStorer {
public:
  virtual ~ValueStorer() = default;

  mutable std::mutex valueMtx;
  ValueOrException<T> value;

  std::atomic<std::size_t> generations = std::atomic<std::size_t>{0};

protected:
  ValueStorer() = default;
};

} // namespace flw
