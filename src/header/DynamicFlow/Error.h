/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <optional>
#include <sstream>
#include <stdexcept>

namespace flw {
class Error : public std::runtime_error {
public:
  explicit Error(const std::string &what);

  template <typename T, typename... Args>
  Error(T front, Args &&...args)
      : Error(Error::merge(front, std::forward<Args>(args)...)){};

private:
  template <typename... Args> static std::string merge(Args &&...args) {
    std::stringstream stream;
    Error::merge_(stream, std::forward<Args>(args)...);
    return stream.str();
  }

  template <typename T, typename... Args>
  static void merge_(std::stringstream &stream, T piece, Args &&...args) {
    Error::merge_(stream, piece);
    Error::merge_(stream, std::forward<Args>(args)...);
  }

  template <typename T> static void merge_(std::stringstream &stream, T piece) {
    stream << piece;
  }
};

class UnsetValueError : public Error {
public:
  UnsetValueError()
      : Error{"Unset value. The reason can be that this is a never set source "
              "or either a value whose re-evaluation can't be done as an "
              "ancestor(s) don't store a value"} {}
};

namespace detail {
template <std::size_t N, typename Exception, typename... Exceptions>
class ExceptionMemoizer : public ExceptionMemoizer<N + 1, Exceptions...> {
protected:
  template <typename Predicate> bool handle_(Predicate &&predicate) {
    try {
      return this->ExceptionMemoizer<N + 1, Exceptions...>::handle_(
          std::forward<Predicate>(predicate));
    } catch (const Exception &e) {
      this->exception_.emplace(e);
    }
    return true;
  }

  void throw_() const {
    if (this->exception_) {
      throw exception_.value();
    }
    this->ExceptionMemoizer<N + 1, Exceptions...>::throw_();
  }

private:
  std::optional<Exception> exception_;
};

template <std::size_t N, typename Exception>
class ExceptionMemoizer<N, Exception> {
protected:
  template <typename Predicate> bool handle_(Predicate &&predicate) {
    try {
      predicate();
      return false;
    } catch (const Exception &e) {
      this->exception_.emplace(e);
    }
    return true;
  }

  void throw_() const {
    if (this->exception_) {
      throw exception_.value();
    }
  }

private:
  std::optional<Exception> exception_;
};
} // namespace detail

/**
 * @brief An object storing a typed exception which can be re-throwned, without
 * loosing the type, not a std::exception is re-throwned.
 */
template <typename... Errors>
class ReThrownable : public detail::ExceptionMemoizer<0, Errors...> {
public:
  ReThrownable() = default;

  /**
   * @brief executes the passed predicate and in case one of the exception part
   * of Errors is throwned, it is stored inside this object and can be
   * re-throwned calling reThrow().
   */
  template <typename Predicate> bool handle(Predicate &&predicate) {
    return this->detail::ExceptionMemoizer<0, Errors...>::handle_(
        std::forward<Predicate>(predicate));
  };

  /**
   * @brief re-throws the last stored exception. In case no expection was
   * stored, nothing actually happens calling this method.
   */
  void reThrow() const {
    this->detail::ExceptionMemoizer<0, Errors...>::throw_();
  }
};
} // namespace flw
