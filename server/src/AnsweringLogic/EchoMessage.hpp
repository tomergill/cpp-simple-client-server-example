/**
 * @file EchoMessage.hpp
 * @author Tomer Gill
 * @brief AnsweringLogic component that echos the message back (for echo servers)
 * @date 2021-01-01 
 */

#ifndef ECHO_MESSAGE_GUARD
#define ECHO_MESSAGE_GUARD

#include "AnsweringLogic.hpp"

class EchoMessage : public AnsweringLogic
{
    /**
     * @brief Returns the message back as a reply
     * 
     * @param message Message received
     * @param length Size of message received
     * @return std::string The same message
     */
    std::string generateReply(char *message, int length) override;
};

#endif