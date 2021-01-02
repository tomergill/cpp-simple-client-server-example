# Simple C++ Client & Server Example
## Background
This is an example for my students in BIU's "Advanced Programming 1 course" (89-210).

It supposed to show them a bit about writing client and servers using sockets.

## The Server
The server is written as a simple TCP server.
Once it set up it's socket, waits for a connection from a client and then starts to receive messages from the client and answering them.

The server's implementation is in the `MyServer` class, while the component that determines the reply is an implementation of the interface `AnsweringLogic`.
Main function is in `server.cpp` file.

Usage: either `MyServer [PORT]` or `MyServer [IP PORT]` where `IP` is the ip the server listens on (default is `127.0.0.1`) and `PORT` is the port the server listens on (default is `54321`, just because)

## The Client
Simple TCP client that connects to a server, sends messages received from `STDIN` and prints the response.
Input is given line-by-line (aka each line is a message).

Usage: `MyClient IP PORT` where `IP` and `PORT` are the server's address and port, respectively