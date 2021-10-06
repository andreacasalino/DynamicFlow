#pragma once

#include <memory>
#include <random>

class Sampler {
public:
  Sampler(const int min, const int max, const long long &seed = 0);

  float sample() const;

private:
  mutable std::default_random_engine generator;

  typedef std::uniform_int_distribution<> Distr;
  typedef std::unique_ptr<std::uniform_int_distribution<>> DistrPtr;
  const DistrPtr distribution;
};
