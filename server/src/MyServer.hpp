// Created by Tomer Gill on 2020-12-31

#ifndef MY_SERVER_GUARD

#define MY_SERVER_GUARD

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define READING_BUFF_SIZE 2048

/**
 * @brief A simple TCP server. Can handle one client at a time (for now)
 * 
 */
class MyServer
{
protected:
    std::string m_ip;
    int m_port, m_socket_fd, m_client_socket_fd;

public:
    /**
     * @brief Construct a new Server object
     * 
     * @param ip Ip address of socket
     * @param port Port of socket
     */
    MyServer(std::string ip = "127.0.0.1", int port = 54321);

    /**
     * @brief Creats a socket and binds it to an address
     * 
     */
    void initialize();

    /**
     * @brief Accepts a new connections from one (1) client and handles the conversation until it finished. Need to call initalize() first
     * 
     */
    void handleOneClient();

    ~MyServer();
};

#endif