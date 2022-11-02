Basic usage of **DynamicFlow**. Shows how to build a small network able to process a buffer of samples and compute statistics on them. A single source is part of the flow and stored the samples to process.

The network is rendered with a python script using graphviz (which is expected to have already been installed with **pip**). Notice that the network rendering is blocking and you should close the generated window and press enter to progress and get to the end.

Notice that a custom exception, `EmptySamplesVector` is registered for one of the node.

Firstly, a valid source of samples is set and the update of the entire flow is done.
Then, the same update of the flow is tried with a bad empty collection of samples with the only purpose of showing that an `EmptySamplesVector` exception will be correctly stored and will prevent the update of some dependant nodes.
