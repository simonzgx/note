cmake_minimum_required(VERSION 2.8)
project(sample_echo_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(sample_echo_server main.cpp src/Dispatcher.cpp include/Dispatcher.h src/TcpConnection.cpp include/TcpConnection.h src/TcpSession.cpp include/TcpSession.h src/Poller.cpp include/Poller.h include/EchoServer.cpp include/EchoServer.h)