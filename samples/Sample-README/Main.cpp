#include <string>

#include <DynamicFlow/Network.h>

int main() {
  // build a new flow
  flw::Flow flow;

  // define the sources nodes
  auto source_1 = flow.makeSource<int>("Source-1");
  auto source_2 = flow.makeSource<std::string>("Source-2");

  // define a node obtained by combining the 2 sources
  auto node = flow.makeNode<std::string, int, std::string>(
      // here you pass or define the lambda used to combine the values of the
      // ancestors, in order to get the new value to store in the node to
      // create.
      [](const int &source_1, const std::string &source_2) {
        // use sources values to get the new node value
        std::string result = source_2 + std::to_string(source_1);
        return result;
      },
      // label to assing to the node (is actually optional)
      "Node",
      // the ancestors
      source_1, source_2);

  // You can attach a callback to the node.
  // This callback will be automatically called by the network everytime this
  // node is updated with a new value.
  node.onNewValueCallBack([](const std::string &new_value) {
    // do something with new_value.
    // You will get here every time a new value is computed for this node
  });

  // every time the value of a generated node is re-computed, the lambda
  // expression passed when creating that node is invoked.
  // Clearly, the execution of that expression may lead to an exception throw.
  // In such cases, no new value is set for the node and the exception throwned
  // is stored in the node and can be re-throwned later.
  // Normally, the exception type would be lost as a generic std::exception is
  // persisted in the node.
  // To overcome this, you can build the node in a slightly different way, in
  // order to tell the flow the kind of custom exceptions to preserve.
  class CustomException1 : public std::runtime_error {
  public:
    CustomException1() : std::runtime_error{"Bla bla"} {};
  };
  class CustomException2 : public std::runtime_error {
  public:
    CustomException2() : std::runtime_error{"Bla bla"} {};
  };
  flow.makeNodeWithMonitoredException<std::string, std::string>(
      [](const std::string &source_2) {
        std::string result;
        // ops ... an exception thrown
        throw CustomException1{};
        return result;
      },
      std::make_unique<
          flw::ValueTypedWithErrors<std::string,
                                    // here you list the exceptions
                                    // for which you don't want to
                                    // loose the type
                                    CustomException1, CustomException2>>(),
      std::nullopt, source_2);

  // update the sources passing their new values. In case of complex
  // classes, you can pass the arguments used by one of the constructor.
  //
  // this will not automatically update the dependant nodes as this will
  // actually happen when updating the flow
  source_1.update(2);
  source_2.update("hello");
  // update those nodes in the flow requiring a recomputation
  flow.update();

  // you can also specify the number of threads to internally use for the update
  flow.setThreads(3);
  // the next update will be done using 3 threads
  flow.update();

  return EXIT_SUCCESS;
}
