// Created by Tomer Gill on 2020-12-31

#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) 
{
    char*               IP = "localhost";
    int                 PORT = 54321;

    int                 socketFd;
    struct sockaddr_in  serverAdress;
    
    int                 clientSocketFd;
    struct sockaddr_in  clientAddress;
    socklen_t           clientAddressLength = sizeof(clientAddress);

    char                buffer[2048];
    int                 bytesRead;

    char*               dataToSend;
    int                 dataToSendLength;
    int                 bytesWritten = 0, totalBytesWritten = 0;


    std::cout << "Hello from the server <3" << std::endl;
    // Note: In a real server all the printing should be log entries instead

    // Create a new IPv4 TCP socket
    socketFd = socket(AF_INET, SOCK_STREAM, NULL);
    if (socketFd < 0)
    {
        throw std::system_error();
    }

    // Prepare the sockaddr_in struct with the server's socket address (AKA ip & port)
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT);  // convert PORT to network order (big endian)
    if (inet_aton(IP, &serverAdress.sin_addr) == 0)
    {
        std::cerr << "ERROR - Address is not valid" << std::endl;
        throw std::system_error();
    }

    // bind() assigns the socket to an IP address and a port
    if (bind(socketFd, (const sockaddr *) &serverAdress, sizeof(serverAdress)) < 0)
    {
        throw std::system_error();
    }

    // Listen for new connections
    // 0 means that no clients will wait in the queue for the server AKA their connections will be refused
    if (listen(socketFd, 0) < 0)
    {
        throw std::system_error();
    }

    // Ooh a new connection! Let's accept it
    // This will create a new socket and establish a TCP connection between it and the client('s socket)
    if ((clientSocketFd = accept(socketFd, (sockaddr *) &clientAddress, &clientAddressLength)) < 0)
    {
        throw std::system_error();
    }

    // Log the client's address
    std::cout << "Got a connection! From " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Read data the client sent. read() call is blocking
    if ((bytesRead = read(clientSocketFd, buffer, 2048)) < 0)  // can also use recv(...)
    {
        /*
         * For simplification the server will terminate if can't read data from the client,
         * but in actual servers we will probably want to only terminate the connection to the client - 
         * the server should continue to serve!
         */
        throw std::system_error();
    }

    /******* LOGIC ******/
    // Echo server, returning what the client sent
    dataToSend = buffer;
    dataToSendLength = bytesRead;
    /****** END LOGIC *****/

    while (totalBytesWritten < dataToSendLength)
    {
        if ((bytesWritten = write(clientSocketFd, dataToSend, dataToSendLength)) < 0)  // can also use send(...)
        {
            throw std::system_error();
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