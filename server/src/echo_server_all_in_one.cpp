/**
 * @file echo_server_all_in_one.cpp
 * @author Tomer Gill
 * @brief Simple Echo server that receives one message and replies it back
 * @date 2020-12-31
 */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define READING_BUFF_SIZE 2048

int main(int argc, char *argv[])
{
    std::string IP = "127.0.0.1";
    int PORT = 54321;

    int socketFd;
    struct sockaddr_in serverAdress;

    int clientSocketFd;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    char buffer[READING_BUFF_SIZE];
    int bytesRead;

    char *dataToSend;
    int dataToSendLength;
    int bytesWritten = 0, totalBytesWritten = 0;

    std::cout << "Hello from the server <3" << std::endl;
    // Note: In a real server all the printing should be log entries instead

    // Create a new IPv4 TCP socket
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
    {
        // This parses ERRNO for us and explains what the error is
        throw std::system_error(errno, std::generic_category(), "Couldn't open a new socket");
    }

    // Prepare the sockaddr_in struct with the server's socket address (AKA ip & port)
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT); // convert PORT to network order (big endian)
    if (inet_aton(IP.c_str(), &serverAdress.sin_addr) == 0)
    {
        std::cerr << "ERROR - Address is not valid" << std::endl;
        return 1;
    }

    // bind() assigns the socket to an IP address and a port
    if (bind(socketFd, (const sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Call to bind() failed");
    }

    // Listen for new connections
    // 0 means that no clients will wait in the queue for the server AKA their connections will be refused
    if (listen(socketFd, 0) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Error when listening to new connections");
    }

    // Ooh a new connection! Let's accept it
    // This will create a new socket and establish a TCP connection between it and the client('s socket)
    if ((clientSocketFd = accept(socketFd, (sockaddr *)&clientAddress, &clientAddressLength)) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Error accepting new connection");
    }

    // Log the client's address
    std::cout << "Got a connection! From " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Read data the client sent. read() call is blocking
    if ((bytesRead = read(clientSocketFd, buffer, READING_BUFF_SIZE)) < 0) // can also use recv(...)
    {
        /*
         * For simplification the server will terminate if can't read data from the client,
         * but in actual servers we will probably want to only terminate the connection to the client - 
         * the server should continue to serve!
         */
        throw std::system_error(errno, std::generic_category(), "Error reading data from connection socket");
    }

    // TODO check if need to add NULL terminator
    // Seems there is no need, but might in the future.
    // Won't implement now cause I'm super lazy 🙃

    std::cout << "Got message: \"" << buffer << "\"!!!" << std::endl;

    /******* LOGIC ******/
    // Echo server, returning what the client sent
    dataToSend = buffer;
    dataToSendLength = bytesRead;
    /****** END LOGIC *****/

    std::cout << "Going to send back message: \"" << dataToSend << "\"" << std::endl;

    while (totalBytesWritten < dataToSendLength)
    {
        if ((bytesWritten = write(clientSocketFd, dataToSend, dataToSendLength)) < 0) // can also use send(...)
        {
            throw std::system_error(errno, std::generic_category(), "Error writing to socket");
        }
        totalBytesWritten += bytesWritten;
    }

    // Finished talking to client? Let's close the socket
    if (close(clientSocketFd) < 0)
    {
        std::cerr << "ERROR - closing client-specific socket failed" << std::endl;
    }

    // Finished with our socket? Let's close it
    if (close(socketFd) < 0)
    {
        std::cerr << "ERROR - closing server's main socket failed" << std::endl;
    }

    // That's all folks! 😁
    return 0;
}