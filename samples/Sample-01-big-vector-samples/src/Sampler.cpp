
#include <Sampler.h>
#include <chrono>

namespace flw::sample {
Sampler::Sampler(const int min, const int max, const long long &seed)
    : distribution(std::make_unique<Distr>(min, max)) {
  if (0 == seed) {
    this->generator.seed(
        std::chrono::system_clock::now().time_since_epoch().count());
  } else {
    this->generator.seed(seed);
  }
};

float Sampler::sample() const {
  return (*this->distribution)(this->generator);
};
} // namespace flw::sample
