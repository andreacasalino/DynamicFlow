/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

namespace flw {

enum EvaluationResult { SUCCESS, NOT_READY, BLOCKING_EXCEPTION };

class EvaluateCapable {
public:
  virtual ~EvaluateCapable() = default;

  virtual EvaluationResult evaluate() = 0;
};

} // namespace flw
