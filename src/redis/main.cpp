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
#include "src/redisclient/redissyncclient.h"

#define LOCAL_HOST "127.0.0.1"
#define PORT 6379

int main() {
    auto address = boost::asio::ip::address::from_string(LOCAL_HOST);
    boost::asio::ip::tcp::endpoint endpoint(address, PORT);
    boost::asio::io_service ioService;
    redisclient::RedisSyncClient redis(ioService);

    boost::system::error_code errCode;
    redis.connect(endpoint, errCode);
    if (errCode) {
        std::cerr << "connect error;" << errCode.message() << std::endl;
        return EXIT_FAILURE;
    }

    redisclient::RedisValue result;

    result = redis.command("SET", {"key", "value"});

    if (result.isError()) {
        std::cerr << "set error" << result.toString() << "\n";
        return EXIT_FAILURE;
    }

    result = redis.command("GET", {"key"});
    if (!result.isOk()) {
        std::cerr << "GET error: " << result.toString() << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "GET: " << result.toString() << "\n";

    redisclient::Pipeline pipeline = redis.pipelined().command("SET", {"name", "simon"})
            .command("SET", {"age", "18"})
            .command("INCR", {"age"});


}