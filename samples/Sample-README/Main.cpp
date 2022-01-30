#include <string>

#include <flow/Flow.h>
using namespace flw;

int main() {
  // build a new flow
  flw::Flow flow;

  // define the sources nodes
  auto source01 = flow.makeSource<int>("Source01");
  auto source02 = flow.makeSource<std::string>("Source02");

  // define a node obtained by combining the 2 sources
  auto node = flow.makeNode<std::string>(
      "Node",
      // here you define the function used to combine the values of the
      // ancestors
      std::function<std::string(const int &, const std::string &)>(
          [](const int &source01_value, const std::string &source02_value) {
            std::string result =
                source02_value + std::to_string(source01_value);
            return "";
          }),
      // here you pass the ancestors
      source01, source02);

  // update the sources passing their new values (they should be unique
  // pointers)
  flow.updateSources(source01.getName(), std::make_unique<int>(2),
                     source02.getName(),
                     std::make_unique<std::string>("hello"));

  // update the entire flow
  flow.updateFlow();

  // you can also update the sources one at a time and then update the flow
  flow.updateSources(source01.getName(), std::make_unique<int>(2));
  flow.updateSources(source02.getName(),
                     std::make_unique<std::string>("hello"));
  flow.updateFlow();

  return EXIT_SUCCESS;
}
