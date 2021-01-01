/**
 * @file client.cpp
 * @author Tomer Gill
 * @brief Simple TCP client
 * @date 2021-01-01 
 */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define RECV_BUFFER_LENGTH 2048

int main(int argc, char *argv[])
{
    std::string ip = "127.0.0.1";
    int port = 54321;
    int socket_fd;
    struct sockaddr_in serverAdress;
    std::string messageToSend;
    const char *dataToSend;
    int dataToSendLength;
    int bytesWritten = 0;
    char buffer[RECV_BUFFER_LENGTH];
    int bytesRead;

    if (argc < 3)
    {
        throw std::invalid_argument("Not enough arguments! Please provide the server's ip and port.\nUsage: MyClient IP PORT");
    }
    ip = argv[1];
    port = atoi(argv[2]);

    std::cout << "Client setting up" << std::endl
              << "Creating Socket..." << std::endl;

    // Create a new IPv4 TCP socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Couldn't open a new socket");
    }

    // Prepare the sockaddr_in struct with the server's socket address (AKA ip & port) so we can connect to it
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port); // convert PORT to network order (big endian)
    if (inet_aton(ip.c_str(), &serverAdress.sin_addr) == 0)
    {
        throw std::invalid_argument("IP Address is not valid");
    }

    // We do not need to bind() as this is not a server
    // A socket address will be assigned to our socket automatically (== the computer's IP address & a random port)

    std::cout << "Setup successful!" << std::endl;

    std::cout << "Connecting to " << ip << ":" << port << "... ";

    if (connect(socket_fd, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Could not connect to server");
    }
    std::cout << "Connected!" << std::endl;

    // Now it is time to send data to the server
    std::cout << "Enter message here and the press ENTER! Enter 'quit' to exit and stop the connection:" << std::endl;
    std::getline(std::cin, messageToSend);
    while (messageToSend != "quit")
    {
        dataToSend = messageToSend.c_str();
        dataToSendLength = messageToSend.length();

        if ((bytesWritten = send(socket_fd, dataToSend, dataToSendLength, 0)) < 0) // can also use write()
        {
            throw std::system_error(errno, std::generic_category(), "Error sending message to client");
        }
        else if (bytesWritten < dataToSendLength)
        {
            std::cerr << "Didn't send all data, only \"" << messageToSend.substr(0, bytesWritten) << "\" (" << bytesWritten << " Bytes)" << std::endl;

            // Won't exit here, the server will reply to whatever data we sent
        }

        bzero(buffer, RECV_BUFFER_LENGTH); // zero the buffer (clear it)
        // Now let's see the server's response...
        if ((bytesRead = recv(socket_fd, buffer, RECV_BUFFER_LENGTH - 1, 0) < 0)) // can also use read()
        {
            throw std::system_error(errno, std::generic_category(), "Error reading server's response");
        }

        std::cout << "Server's reply: \"" << buffer << "\"" << std::endl
                  << std::endl;

        std::cout << "Enter message here and the press ENTER! Empty message (just ENTER) will stop the connection:" << std::endl;
        std::cin >> messageToSend;
    }

    std::cout << "Client shutting down..." << std::endl;

    if (close(socket_fd) < 0)
    {
        std::cerr << "ERROR - closing socket failed: " << std::strerror(errno) << std::endl;
    }

    return 0;
}