#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>

#include <DynamicFlow/Network.hxx>
#include <DynamicFlow/NetworkSerialization.h>

#include <RunScript.h>
#include <Sampler.h>

class EmptySamplesVector : public std::runtime_error {
public:
  EmptySamplesVector() : std::runtime_error{"No samples in vector"} {}
};

std::vector<int> make_samples(std::size_t size) {
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
}

float compute_std_dev(const std::vector<int> &samples, float mean) {
  float variance = 0.f;
  float coeff = 1.f / static_cast<float>(samples.size());
  float diff;
  for (const auto &sample : samples) {
    diff = (sample - mean);
    variance += coeff * diff * diff;
  }
  return sqrtf(variance);
}

int main() {
  //////////////////////////////////////////////////////
  // build a new flow
  flw::Flow flow;
  flow.setOnNewNodePolicy(flw::HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE);

  // define the source, which will be the number of samples to draw
  auto samplesNumber = flow.makeSource<std::size_t>(100000);

  // define the node containing the samples
  // in this case we want to keep track of a particular exception that can be
  // throwned
  bool had_zero_samples = false;
  auto samples = flow.makeNodeWithErrorsCB<std::vector<int>, std::size_t>(
      &make_samples, samplesNumber,
      flw::ValueCallBacks<std::vector<int>, EmptySamplesVector>{}
          .addOnError<EmptySamplesVector>(
              [&had_zero_samples](const EmptySamplesVector &) {
                had_zero_samples = true;
              })
          .extract(),
      "Samples");

  // we are going to register 2 callbacks to catch the results from the network
  float computedMean;
  float computedStdDev;

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
      samples, "Mean", [&computedMean](float val) { computedMean = val; });

  // define the node storing the standard deviation of the samples
  auto samplesStdDev = flow.makeNode<float, std::vector<int>, float>(
      &compute_std_dev, samples, samplesMean, "StdDeviation",
      [&computedStdDev](float val) { computedStdDev = val; });

  // update the flow ... done now as we opted for a
  // flw::HandlerMaker::OnNewNodePolicy::DEFERRED_UPDATE
  flow.update();

  std::cout << "Mean: " << computedMean
            << "   Std deviation: " << computedStdDev << std::endl
            << std::endl;

  // take a snapshot of the network and export it as a .dot file
  flw::Converter<flw::Serialization::DOT>::toFile(
      flw::sample::LogDir::get() / "Flow-Sample-01.dot", flow.snapshot());
  // use python graphviz to render exported .dot file
  flw::sample::RunScript::runDefaultScript("Flow-Sample-01.dot");

  //////////////////////////////////////////////////////
  // update the source with a bad input
  samplesNumber.update(0);
  flow.update();

  // check that the correct exception was stored
  if (had_zero_samples) {
    std::cout << std::endl
              << std::endl
              << "Correctly, a EmptySamplesVector exception was stored"
              << std::endl;
  } else {
    std::cout << std::endl
              << std::endl
              << "Something went wrong storing the exception" << std::endl;
  }

  return EXIT_SUCCESS;
}
