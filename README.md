# DynamicFlow

![binaries_compilation](https://github.com/andreacasalino/DynamicFlow/actions/workflows/installArtifacts.yml/badge.svg)
![binaries_compilation](https://github.com/andreacasalino/DynamicFlow/actions/workflows/runTests.yml/badge.svg)

**DynamicFlow** is a templatized library for handling **data flow network**.
[**Data flow network**](https://en.wikipedia.org/wiki/Data-flow_diagram) are **data flow** having group of variables connected in a directed acyclic graph, whose values are correlated.
Each node of the network contains a value that is obtained by applying a certain specified function to the values previously computed for to the ancestors nodes.
This architecture can be used to model network of variables whose computation absorb a significant amount of time, like rendering engine, machine learning pipeline, long text handling, etc...
The interfaces contained in this repository allows you to create and handle the network, re-setting at the proper time some of the sources in the network.
Then, you can subsequently update the entire network, re-computing only the nodes that were dependent by the updated sources.
The functionalities provided are completely thread-safe (by the way feel free to report any bug if you discover a strange behaviour ;)).

This package is already embedded with the functionality to update the flow by exploiting **multi threading**. From the outside, only the number of threads to use
must be specified.


**Contents**

 * the cores functionalities and sources are contained in ./src
 * ./samples contains 3 classes of examples, extensively showing how to use dynamic flow

**Compile**
   
 * Configure and generate the project using [CMake](https://cmake.org)

   * check this [tutorial](https://www.youtube.com/watch?v=LxHV-KNEG3k) (it is valid for any kind of IDE and not only VS) if you are not familiar
   * When setting the CMake LIB_OPT flag to ON, the libraries that are part of this project will be compiled as shared, otherwise as static. 
	  
 * Compile the library and the samples

 * You can also just download the compiled binaries (compiled in Release) of the library of the latest master commit [here](https://github.com/andreacasalino/DynamicFlow/actions) 
 
**What else?**

If you have found this library useful, please find the time to leave a star :).

![How a data flow looks like:](https://github.com/andreacasalino/DynamicFlow/blob/master/Readme-picture.png)

