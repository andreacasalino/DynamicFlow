The samples shows how to build a collision check pipeline. Indeed, a certain number of convex 2D polygons may change position and orientation over the time. The pipeline check each pair of polygons and determine for every pair:

    * the closest pair of points in case they are not in collision

    * the penetration depth otherwise

The collision checks are done using [**Flexible-GJK-and-EPA**](https://github.com/andreacasalino/Flexible-GJK-and-EPA), which is an external library automatically fetched for you by **CMake** when configuring this sample. The sources of the network are of course the polygon vertices as well as their positions/orientations.
Notice that the number of polygons is parametric and the construction of the entire network is done by `FlowHandler`.

More than one **thread** is used for the network update, as some computations for the pairs of polygons can be done in parallel.

The network is rendered with a python script using graphviz (which is expected to have already been installed with **pip**). Notice that the network rendering is blocking and you should close the generated window and press enter to progress and get to the end.

For each scenario in the sample, the polygons are moved to some specific location and the entire network is updated. After the last containing the results is updated, a results are logged into a **.json** file whose content is rendered with an external python script.
What you will see in the opened window is the set of polygons whose color is set to;

    * green in case that polygon is not in collision with any other one

    * red otherwise

Dashed lines connects the closest points of the pairs not in collsion.

Basic usage of **DynamicFlow**. Shows how to build a small network able to process a buffer of samples and compute statistics on them. A single source is part of the flow and stored the samples to process.
