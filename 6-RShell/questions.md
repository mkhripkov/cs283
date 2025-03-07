1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received from the server by continously reading from the socket until it sees the end of output marker. In my implementation, I loop on recv() calls and check if the final byte is the end of output marker, this accounts for any cases where data arrives in chunks.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Because TCP is a reliable stream protocol, not a message-oriented one, we need our own end of output marker to know where a message starts and ends. If this isn't handled correctly it could result in missing data.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol keeps track of ongoing information about each connection, whereas a stateless protocol doesn't store past information and treats each request as independent of any previous ones.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is generally faster than TCP, so it's used for things like video calls where speed matters more than reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides sockets as the main interface/abstraction for network communication. 