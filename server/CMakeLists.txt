cmake_minimum_required(VERSION 3.10.0)
project(MyServer)

add_executable(MyServer src/server.cpp src/MyServer.cpp src/MyServer.hpp src/AnsweringLogic/EchoMessage.cpp src/AnsweringLogic/AnsweringLogic.hpp src/AnsweringLogic/EchoMessage.hpp)