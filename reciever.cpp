#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char const *filename = "received.txt";
    std::ofstream file(filename, std::ios::binary);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "Setsockopt error" << std::endl;
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Listen error" << std::endl;
        return 1;
    }

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        std::cerr << "Accept error" << std::endl;
        return 1;
    }

    // Receive file contents
    char buffer[1024] = {0};
    int n;
    while ((n = read(new_socket, buffer, sizeof(buffer))) > 0)
    {
        file.write(buffer, n);
        memset(buffer, 0, sizeof(buffer));
    }

    std::cout << "File received successfully" << std::endl;

    // Close sockets and file
    close(new_socket);
    close(server_fd);
    file.close();
    return 0;
}
