//
// Created by Simon on 2020/3/25.
//

#include <string>
#include <vector>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>

#define LOCAL_HOST "127.0.0.1"
#define PORT 6379

int main(){
    auto address = boost::asio::ip::address::from_string(LOCAL_HOST);
    boost::asio::ip::tcp::endpoint endpoint(address, PORT);
    boost::asio::io_service ioService;

}