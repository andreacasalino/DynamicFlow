A simple network with no particular meaning is built with the purpose to show how to thread-safely interact with **DynamicFlow**.

In essence:
- one thread continously updated the flow
- two different threads continously update two distinct sources
- values of the nodes are monitored and periodically logged to file

Check the comments in the code for further details.
