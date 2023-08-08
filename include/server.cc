#include "server.hh"

#include "handler.hh"

#include <semaphore>
#include <memory>
#include <thread>
#include <iostream>
#include <functional>
#include <utility>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace potion {
Server::Server (int port) : PORT {port},
                    handler_{} {}

void Server::start () {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int address_length = sizeof(struct sockaddr_in);

    if(server_fd == 0) {
        std::cout << "Socket Error";
        return;
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET, 
        .sin_port = htons(PORT),
    };

    address.sin_addr.s_addr = INADDR_ANY;

    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr* )&address, sizeof(address)) < 0) {
        std::cout << "Bind Error";
        return;
    }

    if (listen(server_fd, 10) < 0) {
        std::cout << "Listen error";
        return;
    }
    
    while (true) {
        int request_fd = accept(server_fd, (struct sockaddr * )&address, (socklen_t*)&address_length);
        std::thread handle_connection(
            [this](int a){
                this->handler_.handle_connection(a);
            }, request_fd);
        handle_connection.detach();
    }
}
}
