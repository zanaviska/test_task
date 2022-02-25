#include <iostream>
#include <string>

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "sm_receiver.h"
#include "sm_sender.h"

#define MAX 80
#define PORT 8080

class tcp_server
{
    int sockfd;
    int connfd;

public:
    // create server and binds the socket
    tcp_server()
    {
        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            std::cerr << "socket creation failed...\n";
            throw std::exception();
        }

        sockaddr_in servaddr, cli;

        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr))) != 0)
        {
            close(sockfd);

            std::cerr << "socket bind failed...\n";
            throw std::exception();
        }
    }

    // accept new connection
    // return false if connection was failed
    bool accept_connection()
    {
        if ((listen(sockfd, 5)) != 0)
        {
            std::cerr << "Listen failed...\n";
            return false;
        }

        sockaddr cli;
        int len = sizeof(cli);

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (sockaddr *)&cli, (socklen_t *)&len);
        if (connfd < 0)
        {
            std::cerr << "server accept failed...\n";
            return false;
        }

        return true;
    }

    // receives responce from the server
    std::string receive()
    {
        std::string res(MAX, 0);

        int ln = read(connfd, res.data(), res.length());
        res.resize(ln);

        return res;
    }

    // sends string to the server
    void send(const std::string &message) { write(connfd, message.data(), message.length()); }

    ~tcp_server() { close(sockfd); }
};

// Driver function
int main()
{
    std::cerr << "This is server\n";

    tcp_server server;
    sm_receiver receiver(SHM_KEY_1);
    sm_sender sender(SHM_KEY_2);

    while (true)
    {
        if (!server.accept_connection())
        {
            std::cerr << "failed to accept connection";
            return -1;
        }
        while (true)
        {
            auto request = server.receive();
            if (request.empty())
            {
                std::cout << "connection breaked\n";
                break;
            }
            std::cout << "client said: " << request << '\n';
            sender.send(request);

            std::string responce = receiver.receive();
            std::cout << "server replied: " << responce << '\n';

            server.send(responce);
        }
    }
}