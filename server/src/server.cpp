// Created by Tomer Gill on 2020-12-31

#include <iostream>
#include "MyServer.hpp"

#define READING_BUFF_SIZE 2048

int main(int argc, char *argv[])
{
    MyServer server;

    server.initialize();
    server.handleOneClient();

    // That's all folks! 😁
    return 0;
}