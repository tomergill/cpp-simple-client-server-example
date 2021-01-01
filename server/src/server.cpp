// Created by Tomer Gill on 2020-12-31

#include <iostream>
#include "MyServer.hpp"

#define READING_BUFF_SIZE 2048

int main(int argc, char *argv[])
{
    MyServer server;

    server.initialize();

    std::cout << "Will now accept connection from a client" << std::endl;
    server.handleOneClient();

    std::cout << "Finished handling client(s). Server shutting down..." << std::endl;

    // That's all folks! 😁
    return 0;
}