1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines the end of a command's output by checking for a special EOF character or a message delimiter sent by the server. Techniques to handle partial reads include looping over recv() until all expected data is received and using timeouts to prevent hanging.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should define a clear message termination character (i.e. \n or \0) or fixed-length headers to indicate message size. Without proper handling, data can be split across multiple packets, causing incomplete reads, or messages may merge together, making parsing difficult.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol keeps track of past interactions (e.g., TCP, where connections are maintained). A stateless protocol treats each request independently, without storing session data (like HTTP). Stateful protocols require more resources but allow continuous interactions, while stateless protocols are simpler and scale better.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is used because it is faster and has lower overhead than TCP. It is useful for applications where speed matters more than reliability, such as video streaming, online gaming, and real-time communications, where occasional data loss is acceptable.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides sockets as the primary interface for network communication. The Sockets API (used in C, Python, etc.) allows applications to create, send, receive, and manage network connections using TCP or UDP.