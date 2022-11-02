#pragma once

#include <memory>
#include <random>

namespace flw::sample {
class Sampler {
public:
  Sampler(const int min, const int max, const long long &seed = 0);

  float sample() const;

private:
  mutable std::default_random_engine generator;

  using Distr = std::uniform_int_distribution<>;
  using DistrPtr = std::unique_ptr<std::uniform_int_distribution<>>;
  const DistrPtr distribution;
};
} // namespace flw::sample
