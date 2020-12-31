/**
 * @file MyServer.cpp
 * @author Tomer Gill
 * @date 2021-01-01
 */

#include "MyServer.hpp"

MyServer::MyServer(std::string ip, int port) : m_ip(ip), m_port(port), m_socket_fd(-1), m_client_socket_fd(-1) {}

void MyServer::initialize()
{
    struct sockaddr_in serverAdress;
    std::cout << "Server setting up" << std::endl
              << "Creating socket...";

    // Create a new IPv4 TCP socket
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd < 0)
    {
        std::cerr << "Couldn't open a new socket" << std::endl;
        throw std::system_error();
    }

    // Prepare the sockaddr_in struct with the server's socket address (AKA ip & port)
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(m_port); // convert PORT to network order (big endian)
    if (inet_aton(m_ip.c_str(), &serverAdress.sin_addr) == 0)
    {
        throw std::invalid_argument("Address is not valid");
    }

    std::cout << "Binding socket to address & port..." << std::endl;

    // bind() assigns the socket to an IP address and a port
    if (bind(m_socket_fd, (const sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
    {
        std::cerr << "Call to bind() with IP=" << m_ip << " and port=" << m_port << " failed" << std::endl;
        throw std::system_error();
    }

    std::cout << "Setup successful! Can now handle connections on address " << m_ip << ":" << m_port << std::endl;
}

MyServer::~MyServer()
{
    if (close(m_socket_fd) < 0)
    {
        std::cerr << "ERROR - closing server's main socket failed" << std::endl;
    }
}

void MyServer::handleOneClient()
{
    if (m_socket_fd < 0)
    {
        throw std::logic_error("Server's socket not initialized!! Need to call initialize() before accepting connections");
    }

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    char buffer[READING_BUFF_SIZE];
    int bytesRead;

    char *dataToSend;
    int dataToSendLength;
    int bytesWritten = 0, totalBytesWritten = 0;

    // Listen for new connections
    // 0 means that no clients will wait in the queue for the server AKA their connections will be refused
    if (listen(m_socket_fd, 0) < 0)
    {
        throw std::system_error();
    }

    // Ooh a new connection! Let's accept it
    // This will create a new socket and establish a TCP connection between it and the client('s socket)
    if ((m_client_socket_fd = accept(m_socket_fd, (sockaddr *)&clientAddress, &clientAddressLength)) < 0)
    {
        throw std::system_error();
    }

    // Log the client's address
    std::cout << "Got a connection! From " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Read data the client sent. read() call is blocking
    if ((bytesRead = read(m_client_socket_fd, buffer, READING_BUFF_SIZE)) < 0) // can also use recv(...)
    {
        /*
            * For simplification the server will terminate if can't read data from the client,
            * but in actual servers we will probably want to only terminate the connection to the client - 
            * the server should continue to serve!
            */
        throw std::system_error();
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
        if ((bytesWritten = write(m_client_socket_fd, dataToSend, dataToSendLength)) < 0) // can also use send(...)
        {
            throw std::system_error();
        }
        totalBytesWritten += bytesWritten;
    }

    // Finished talking to client? Let's close the socket
    if (close(m_client_socket_fd) < 0)
    {
        std::cerr << "ERROR - closing client-specific socket failed" << std::endl;
    }
}