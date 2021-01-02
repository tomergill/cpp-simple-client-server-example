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
              << "Creating socket..." << std::endl;

    // Create a new IPv4 TCP socket
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd < 0)
    {
        // This exception parses ERRNO for us and explains what the error is
        throw std::system_error(errno, std::generic_category(), "Couldn't open a new socket");
    }

    // Prepare the sockaddr_in struct with the server's socket address (AKA ip & port)
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(m_port); // convert PORT to network order (big endian)
    if (inet_aton(m_ip.c_str(), &serverAdress.sin_addr) == 0)
    {
        throw std::invalid_argument("IP Address is not valid");
    }

    std::cout << "Binding socket to address & port..." << std::endl;

    // bind() assigns the socket to an IP address and a port
    if (bind(m_socket_fd, (const sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Call to bind() failed");
    }

    std::cout << "Setup successful! Can now handle connections on address " << m_ip << ":" << m_port << std::endl;
}

MyServer::~MyServer()
{
    // In dtor we close/release all the resources our object is in charge of - in our case, the server's main socket
    // This is in accordance to RAII (https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization)
    if (close(m_socket_fd) < 0)
    {
        std::cerr << "ERROR - closing server's main socket failed: " << std::strerror(errno) << std::endl;
    }
}

void MyServer::handleOneClient(AnsweringLogic &logic)
{
    if (m_socket_fd < 0)
    {
        throw std::logic_error("Server's socket not initialized!! Need to call initialize() before accepting connections");
    }

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    char buffer[READING_BUFF_SIZE];
    int bytesRead;

    const char *dataToSend;
    int dataToSendLength;
    int bytesWritten = 0, totalBytesWritten = 0;

    std::string reply;

    // Listen for new connections
    // 0 means that no clients will wait in the queue for the server AKA their connections will be refused
    if (listen(m_socket_fd, 0) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Error when listening to new connections");
    }

    // Ooh a new connection! Let's accept it
    // This will create a new socket and establish a TCP connection between it and the client('s socket)
    if ((m_client_socket_fd = accept(m_socket_fd, (sockaddr *)&clientAddress, &clientAddressLength)) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "Error accepting new connection");
    }

    // Log the client's address
    std::cout << "Got a connection! From " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    while (true)
    {
        bzero(buffer, READING_BUFF_SIZE); // zero the buffer (clean it)
        // Read data the client sent. read() call is blocking
        if ((bytesRead = read(m_client_socket_fd, buffer, READING_BUFF_SIZE)) < 0) // can also use recv(...)
        {
            /*
                * For simplification the server will terminate if can't read data from the client,
                * but in actual servers we will probably want to only terminate the connection to the client - 
                * the server should continue to serve!
                */
            throw std::system_error(errno, std::generic_category(), "Error reading from connection socket");
        }
        else if (bytesRead == 0)
        {
            // Read no bytes - either connection has closed or client taking too long
            // Let's exit
            break; // Why break and not return? because we still need to close the client socket
        }

        // TODO check if need to add NULL terminator
        // Seems there is no need, but might in the future.
        // Won't implement now cause I'm super lazy 🙃

        std::cout << "Got message: \"" << buffer << "\"!!!" << std::endl;

        // We let our AnsweringLogic decide what to reply. Each call to handleOneClient can use a different Answering logic without changing the server code!
        reply = logic.generateReply(buffer, bytesRead);
        dataToSend = reply.c_str();
        dataToSendLength = reply.size();

        std::cout << "Going to send back message: \"" << dataToSend << "\"" << std::endl;

        // There is a possibility that not all data will be sent, so we want to continue send it until finishes
        while (totalBytesWritten < dataToSendLength)
        {
            if ((bytesWritten = write(m_client_socket_fd, dataToSend, dataToSendLength)) < 0) // can also use send(...)
            {
                throw std::system_error(errno, std::generic_category(), "Error writing data to socket");
            }
            totalBytesWritten += bytesWritten;
        }
    }

    // Finished talking to client? Let's close the socket
    if (close(m_client_socket_fd) < 0)
    {
        std::cerr << "ERROR - closing client-specific socket failed: " << std::strerror(errno) << std::endl;
    }
}