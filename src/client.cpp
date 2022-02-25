#include <iostream>

#include <arpa/inet.h>

#include "json.hpp"

#define MAX 80
#define PORT 8080

class tcp_client
{
    int sockfd;

public:
    // create object and open socket for connection
    tcp_client()
    {
        int connfd;
        sockaddr_in cli;

        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            std::cerr << "socket creation failed...\n";
            throw std::exception();
        }
    }

    // connects to the server with ip and port
    // return false on failed connection
    bool connect(const std::string &ip, int port)
    {
        sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ip.data());
        servaddr.sin_port = htons(port);
        return ::connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) == 0;
    }

    // sends string to the server
    void send(const std::string &message) { write(sockfd, message.data(), message.length()); }

    // receives responce from the server
    std::string receive()
    {
        std::string res(MAX, 0);

        int ln = read(sockfd, res.data(), res.length());
        res.resize(ln);

        return res;
    }
    ~tcp_client()
    {
        // close the socket
        close(sockfd);
    }
};

int main()
{
    tcp_client client;
    if (!client.connect("127.0.0.1", PORT))
    {
        std::cerr << "failed connect to the server\n";
        return -1;
    }

    while (1)
    {
        std::string company;
        std::cout << "Company: ";
        // std::cin.ignore(0);
        do
        {
            std::getline(std::cin, company);
        } while (company.empty());
       
        std::string symbol;
        std::cout << "Symbol: ";
        // std::cin.ignore(0);
        do
        {
            std::getline(std::cin, symbol);
        } while (symbol.empty());
       
        int ordered;
        std::cout << "Number ordered: ";
        std::cin >> ordered;
       
        nlohmann::json j;
        j["symbol"] = symbol;
        j["company"] = company;
        j["numOrdered"] = ordered;

        client.send(j.dump());
        std::cout << "Price: " << client.receive() << "\n\n";
    }
}