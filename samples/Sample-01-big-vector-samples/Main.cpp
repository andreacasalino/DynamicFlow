#include <Sampler.h>
#include <flow/Flow.h>

#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>
using namespace flw;

int main() {
  // build a new flow object
  flw::Flow flow;

  // define the source, which will be the number of samples to draw
  auto samplesNumber = flow.makeSource<std::size_t>("SamplesNumber");

  // define the node containing the samples
  auto samples =
      flow.makeNode("Samples",
                    std::function<std::vector<int>(const std::size_t &)>(
                        [](const std::size_t &size) {
                          Sampler sampler(-5, 5);
                          std::vector<int> samples;
                          samples.reserve(size);
                          for (std::size_t k = 0; k < size; ++k) {
                            samples.push_back(sampler.sample());
                          }
                          return samples;
                        }),
                    samplesNumber);

  // define the node storing the mean of the samples
  auto samplesMean =
      flow.makeNode("SamplesMean",
                    std::function<float(const std::vector<int> &)>(
                        [](const std::vector<int> &samples) {
                          if (samples.empty()) {
                            throw Error("Empty samples vector");
                          }

                          float mean = 0.f;
                          float coeff =
                              1.f / static_cast<float>(samples.size());
                          for (const auto &sample : samples) {
                            mean += coeff * sample;
                          }
                          return mean;
                        }),
                    samples);

  // define the node storing the standard deviation of the samples
  auto samplesStdDev = flow.makeNode(
      "SamplesStdDev",
      std::function<float(const std::vector<int> &, const float &)>(
          [](const std::vector<int> &samples, const float &mean) {
            float variance = 0.f;
            float coeff = 1.f / static_cast<float>(samples.size());
            float diff;
            for (const auto &sample : samples) {
              diff = (sample - mean);
              variance += coeff * diff * diff;
            }
            return sqrtf(variance);
          }),
      samples, samplesMean);

  // update the source to trigger the flow update
  flow.updateSourcesAndFlow(samplesNumber.getName(),
                            std::make_unique<std::size_t>(100000));
  std::cout << "Mean: " << copyValue(samplesMean)
            << "   Std deviation: " << copyValue(samplesStdDev) << std::endl;

  // update the source with a bad input will lead to exception throw inside the
  // flow update
  flow.updateSourcesAndFlow(samplesNumber.getName(),
                            std::make_unique<std::size_t>(0));
  if (samplesMean.isException()) {
    try {
      std::rethrow_exception(samplesMean.getException());
    } catch (const std::exception &e) {
      std::cout << "samplesMean contains exception: " << e.what() << std::endl;
    }
  }
  if (samplesStdDev.isException()) {
    try {
      std::rethrow_exception(samplesStdDev.getException());
    } catch (const std::exception &e) {
      std::cout << "samplesStdDev contains exception: " << e.what()
                << std::endl;
    }
  }

  return EXIT_SUCCESS;
}
