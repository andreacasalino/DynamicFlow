#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>

#include <DynamicFlow/Network.h>
#include <DynamicFlow/NetworkIO.h>

#include <RunScript.h>
#include <Sampler.h>

class EmptySamplesVector : public std::runtime_error {
public:
  EmptySamplesVector() : std::runtime_error{"No samples in vector"} {}
};

int main() {
  //////////////////////////////////////////////////////
  // build a new flow
  flw::Flow flow;

  // define the source, which will be the number of samples to draw
  auto samplesNumber = flow.makeSource<std::size_t>("SamplesNumber");

  // define the node containing the samples
  //
  // in this case we want to keep track of a particular exception that can be
  // throwned
  auto samples_value = std::make_unique<
      flw::ValueTypedWithErrors<std::vector<int>, EmptySamplesVector>>();
  auto samples =
      flow.makeNodeWithMonitoredException<std::vector<int>, std::size_t>(
          [](const std::size_t &size) {
            if (0 == size) {
              throw EmptySamplesVector{};
            }
            flw::sample::Sampler sampler(-5, 5);
            std::vector<int> samples;
            samples.reserve(size);
            for (std::size_t k = 0; k < size; ++k) {
              samples.push_back(sampler.sample());
            }
            return samples;
          },
          std::move(samples_value), "Samples", samplesNumber);

  // define the node storing the mean of the samples
  auto samplesMean = flow.makeNode<float, std::vector<int>>(
      [](const std::vector<int> &samples) {
        float mean = 0.f;
        float coeff = 1.f / static_cast<float>(samples.size());
        for (const auto &sample : samples) {
          mean += coeff * sample;
        }
        return mean;
      },
      "SamplesMean", samples);

  // define the node storing the standard deviation of the samples
  auto samplesStdDev = flow.makeNode<float, std::vector<int>>(
      [](const std::vector<int> &samples, const float &mean) {
        float variance = 0.f;
        float coeff = 1.f / static_cast<float>(samples.size());
        float diff;
        for (const auto &sample : samples) {
          diff = (sample - mean);
          variance += coeff * diff * diff;
        }
        return sqrtf(variance);
      },
      "SamplesStdDev", samples, samplesMean);

  // register 2 callback to catch the results from the network
  float computedMean;
  samplesMean.onNewValueCallBack(
      [&computedMean](const int &v) { computedMean = v; });
  float computedStdDev;
  samplesStdDev.onNewValueCallBack(
      [&computedStdDev](const int &v) { computedStdDev = v; });

  //////////////////////////////////////////////////////
  // update the sources and the flow
  samplesNumber.update(static_cast<std::size_t>(100000));
  flow.update();
  std::cout << "Mean: " << computedMean
            << "   Std deviation: " << computedStdDev << std::endl
            << std::endl;

  // take a snapshot of the network and export it as a .dot file
  flw::to_dot("Flow-Sample-01.dot", flow.getSnapshot(true));
  // use python graphviz to render exported .dot file
  flw::sample::runShowGraph("Flow-Sample-01.dot");

  // update the source with a bad input
  samplesNumber.update(static_cast<std::size_t>(0));
  flow.update();
  // check that the correct exception was stored
  try {
    samples.getValue().reThrow();
  } catch (const EmptySamplesVector &) {
    std::cout << std::endl
              << std::endl
              << "Correctly, a EmptySamplesVector exception was stored"
              << std::endl;
  } catch (...) {
    std::cout << std::endl
              << std::endl
              << "Something went wrong storing the exception" << std::endl;
  }
  // all dependant nodes should be left as unset
  if ((samplesMean.getValue().status() == flw::ValueStatus::UNSET) &&
      (samplesStdDev.getValue().status() == flw::ValueStatus::UNSET)) {
    std::cout << "Dependant nodes were left unset" << std::endl << std::endl;
  }

  return EXIT_SUCCESS;
}
