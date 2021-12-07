/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

namespace flw {

/**
 * @brief Results of an internal evaluation.
 * SUCCESS -> the evaluation was done without problems
 * NOT_READY -> it is not possible to do the evaluation in this moment
 * BLOCKING_EXCEPTION -> it is not possible to do the evaluation as a dependant
 * object stores an exception
 */
enum class EvaluationResult {
  SUCCESS = 1,
  NOT_READY = 2,
  BLOCKING_EXCEPTION = 3
};

/**
 * @brief An object that can be update by evaluating an internal expression
 */
class EvaluateCapable {
public:
  virtual ~EvaluateCapable() = default;

  /**
   * @brief Tries to evaluate an internal expression.
   */
  virtual EvaluationResult evaluate() = 0;

protected:
  EvaluateCapable() = default;
};

} // namespace flw
