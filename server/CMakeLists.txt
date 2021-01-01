cmake_minimum_required(VERSION 3.10.0)
# project(MyServer)

add_executable(MyServer src/server.cpp src/MyServer.cpp src/AnsweringLogic/EchoMessage.cpp)
target_include_directories(MyServer PUBLIC src)  # where to look for .h/.hpp files