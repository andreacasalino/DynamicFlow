/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/TypeTraits.h>
#include <DynamicFlow/Value.h>

namespace flw {
template <typename T, typename... ErrorsT>
void Value<T, ErrorsT...>::update(std::function<T()> pred) {
  this->value_.reset();
  try {
    if constexpr (sizeof...(ErrorsT) == 0) {
      this->value_.emplace(pred());
      ++this->generation_;
      this->onValue(this->value_.value());
    } else {
      this->update_<ErrorsT...>(pred);
    }
  } catch (const std::exception &e) {
    this->template onError<std::exception>(e);
  }
}

template <typename T, typename... ErrorsT>
template <typename ErrorFirst, typename... ErrorsRest>
void Value<T, ErrorsT...>::update_(const std::function<T()> &pred) {
  try {
    if constexpr (sizeof...(ErrorsRest) == 0) {
      this->value_.emplace(pred());
      ++this->generation_;
      this->onValue(this->value_.value());
    } else {
      this->update_<ErrorsRest...>(pred);
    }
  } catch (const ErrorFirst &e) {
    this->template onError<ErrorFirst>(e);
  }
}
} // namespace flw
