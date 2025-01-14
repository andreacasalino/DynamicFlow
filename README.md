# DynamicFlow

![binaries_compilation](https://github.com/andreacasalino/DynamicFlow/actions/workflows/runTests.yml/badge.svg)

![How a data flow looks like:](Readme-picture.png)

The code quality is kept tracked by [SonarCloud](https://sonarcloud.io/summary/new_code?id=andreacasalino_DynamicFlow).

- [What is this library about](#intro)
- [Features](#features)
- [Usage](#usage)
- [CMake support](#cmake-support)

## INTRO

Haven't left a **star** already? Do it now ;)!

**DynamicFlow** is a heavily templatized library for handling **data flow programming**.
[**Data flow programming**](https://devopedia.org/dataflow-programming) refers to the generation and handling of **data flow** having group of variables connected in a directed acyclic graph.
Each node of the network contains a value that is obtained by applying a certain specified function to the values previously computed for to the ancestors nodes.
This architecture can be used to model network of variables whose computation absorb a significant amount of time, like rendering engine, machine learning pipeline, long text handling, etc...
The shape of the dependencies is exploited in order to enforce lazyness, re-computing the least possible number of nodes, every time that one or more sources of the network are update.

**DynamicFlow** allows you to create and handle the entities of the network, re-setting at the proper time the sources.
Then, you can subsequently update the entire network, re-computing only the nodes that were dependent by the updated sources.

### CONTENT

 * the sources of the library are contained in the [src](./src) folder.

 * example of usage are contained in the [samples](./samples) folder. Refer to the specific **README** of each sample:

    - [samples/Sample-01-big-vector-samples/README.md](samples/Sample-01-big-vector-samples/README.md)
    - [samples/Sample-02-long-text-process/README.md](samples/Sample-02-long-text-process/README.md)
    - [samples/Sample-03-concurrent-update-read/README.md](samples/Sample-03-concurrent-update-read/README.md)
    - [samples/Sample-04-collisions-detection/README.md](samples/Sample-04-collisions-detection/README.md)

## FEATURES

Haven't left a **star** already? Do it now ;)!

These are the most notable properties of **DynamicFlow**:

* **DynamicFlow** relies on modern **C++** and generic programming to give you a nice interface to generate the nodes that are part of the flow. Indeed, you just need to specify the lambda expression to use every time for generating a new value and the list of ancestors node, see also the [Usage](#usage) Section.

* **DynamicFlow** is be completely thread-safe. You can also update a network while adding new sources/nodes. By the way, feel free to report any bug if you discover a strange behaviour ;).

* More than one thread can be used for updating the entire network. From the outside, only the number of threads to use must be specified.

* For each node af a network, it is possible to register specific exception type that should be saved if an exception occour while evaluating the lambda expression associated to the node. Later, such exceptions can be rethrowned wihtout losing the exception type. See also the [Usage](#usage) Section.

* You can export a dat flow network built with **DynamicFlow** into a:
    * **.json** file
    * **.dot** file, which can be rendered for example by making use of [Graphviz](https://graphviz.org/). The samples inside this repo already do something similar using [this](samples/python/ShowGraph.py) **Python** script.

## USAGE

Haven't left a **star** already? Do it now ;)!

You can build a dat flow step by step, defining each sources/nodes the flow should be made of.
Creating the sources of the network can be easily done on this way:
```cpp
#include <DynamicFlow/Network.hxx>

// build a new flow
flw::Flow flow;

// Define the sources nodes, passing the initial values
auto source_1 = flow.makeSource<int>(576, "Source-1");
auto source_2 = flow.makeSource<std::string>("Some value", "Source-2");
```

Now you are ready to create the first node:
```cpp
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
```

Every time the value of a generated node is re-computed, the lambda expression passed when creating that node is invoked.
Clearly, the execution of that expression may lead to an exception throw.
In order to react to such circumnstances, **DynamicFlow** allows you to register a set of callbacks for the different kind of exception that may be thrown:
```cpp
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
```

Values for the sources can be update, in order to trigger the required update on the other nodes of the network:
```cpp
// Now is the time to update one of the source (we could update multiple ones or them all if needed).
source_2.update("Some other value");
// Update those nodes in the flow requiring a recomputation
flow.update();
```

You can also tell **DynamicFlow** to use multiple threads to make the update of the entire network:
```cpp
// You can also decide to update the flow, using multiple threads.
// This is actually recommended only for very big network.
flow.setThreads(3);
// 3 threads will be used by the next update as 3 was passed to setThreads(...)
flow.update();
```

## CMAKE SUPPORT
   
Haven't yet left a **star**? Do it now! ;).

To consume this library you can rely on [CMake](https://cmake.org).
More precisely, You can fetch this package and link to the **DynamicFlow** library:
```cmake
include(FetchContent)
FetchContent_Declare(
dyn_flow
GIT_REPOSITORY https://github.com/andreacasalino/DynamicFlow
GIT_TAG        main
)
FetchContent_MakeAvailable(dyn_flow)
```

and then link to the **DynamicFlow** library:
```cmake
target_link_libraries(${TARGET_NAME}
   DynamicFlow
)
```

By default, only the functionalities to export generated data flow into **.dot** file are enabled. 
Instead, convertions to **json** are not. In order to enable it, you should turn to **ON** the camke option named **DYNAMIC_FLOW_ENABLE_JSON_EXPORT**. Notice that such a functionality relies on the well known [**nlohmann**](https://github.com/nlohmann/json) library, which automatically fetched from github for you by **CMake**.
