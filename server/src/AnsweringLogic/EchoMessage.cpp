#include "EchoMessage.hpp"

std::string EchoMessage::generateReply(char *message, int length)
{
     return std::string(message, length);
}