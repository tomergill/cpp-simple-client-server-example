/**
 * @file AnsweringLogic.hpp
 * @author Tomer Gill
 * @brief Abstract class (interface) holding the server replying component (content-wise). 
 * @date 2021-01-01
 */

#ifndef ANSWERING_LOGIC_GUARD

#define ANSWERING_LOGIC_GUARD

#include <iostream>

/**
 * @brief This is a Strategy pattern abstract class. 
 * The server will hold a pointer to the abstract class, while the implementation will hold the "brains" (that will decide what to answer the client).
 * At any time the server can decide to change implementations with minimum code change
 * 
 */
class AnsweringLogic 
{
public:
    /**
     * @brief Decides on a reply based on a message received. Currently limited to string replies
     * 
     * @param receivedMessage The received message from the client
     * @param length Size of message
     * @return std::string The reply
     */
    virtual std::string generateReply(char* receivedMessage, int length) = 0;
};

#endif