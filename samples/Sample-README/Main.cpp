#include <string>

#include <DynamicFlow/Network.hxx>

int main() {
  // Build a new empty flow
  flw::Flow flow;

  // Define the sources nodes, passing the initial values
  auto source_1 = flow.makeSource<int>(576, "Source-1");
  auto source_2 = flow.makeSource<std::string>("Some value", "Source-2");

  // Define a node combining the 2 sources
  // The intial value for the node will be computed right after built, as the default policy of the network
  // is OnNewNodePolicy::IMMEDIATE_UPDATE
  auto node = flow.makeNode<std::string, int, std::string>(
      // here you pass or define the lambda used to processing the values of the ancestors
      [](const int &source_1, const std::string &source_2) {
        // use sources values to get the new node value
        std::string result = source_2 + std::to_string(source_1);
        return result;
      },
      // the ancestors
      source_1, source_2,
      // label to assing to the node (is actually optional ... needed if you want to retrieve the node later from the network)
      "Node", 
      // we want in this case to also register a call back, triggered every time a new value is computed for the node
      [](const std::string &new_value) {
        // do something fancy with new_value ... 
      });

  // It may happen that an exception is thrown when executing the labda passed to a node, when trying 
  // to update the value stored in that node.
  // You can register a call back for each specific exception type that can be thrown.
  // In this way, the call back of the exception is given the exception without loosing its type.
  class CustomException1 : public std::runtime_error {
  public:
    CustomException1() : std::runtime_error{"Bla bla"} {};
  };
  class CustomException2 : public std::runtime_error {
  public:
    CustomException2() : std::runtime_error{"Bla bla"} {};
  };
  flow.makeNodeWithErrorsCB<std::string, std::string>(
      [](const std::string &source_2) {
        std::string result;
        // ops ... an exception is thrown
        throw CustomException1{};
        return result;
      },
      source_2,
      flw::ValueCallBacks<std::string, CustomException1, CustomException2>{}
      .addOnValue([](const std::string& val) {
        // do something with the value
      })
      .addOnError<CustomException1>([](const CustomException1& e) {
        // inspect e and trigger the proper reaction
      })
      .addOnError<CustomException2>([](const CustomException2& e) {
        // inspect e and trigger the proper reaction
      })
      .extract());

  // Now is the time to update one of the source (we could update multiple ones or them all if needed).
  source_2.update("Some other value");
  // Update those nodes in the flow requiring a recomputation
  flow.update();

  // You can also decide to update the flow, using multiple threads.
  // This is actually recommended only for very big network.
  flow.setThreads(3);
  // 3 threads will be used by the next update as 3 was passed to setThreads(...)
  flow.update();

  return EXIT_SUCCESS;
}
