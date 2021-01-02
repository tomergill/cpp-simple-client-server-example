// Created by Tomer Gill on 2020-12-31

#include <iostream>
#include "MyServer.hpp"
#include "AnsweringLogic/EchoMessage.hpp"

#define READING_BUFF_SIZE 2048

/**
 * @brief Main function for server: creates a new echo server and handles a connection with one client
 * 
 * @param argv Can be given ip & port to bind to (MyServer IP PORT), just the port with default ip of localhost (MyServer PORT) or without arguments using the default localhost:54321 (MyServer)
 */
int main(int argc, char *argv[])
{
    EchoMessage logic;
    MyServer server;

    if (argc == 3)
    {
        server = MyServer(argv[1], atoi(argv[2])); // ip, port
    }
    else if (argc == 2)
    {
        server = MyServer("127.0.0.1", atoi(argv[1])); // ip, port
    }

    server.initialize();

    std::cout << "Will now accept connection from a client" << std::endl;
    server.handleOneClient(logic);

    std::cout << "Finished handling client(s). Server shutting down..." << std::endl;

    // That's all folks! 😁
    return 0;
}