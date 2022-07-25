#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#define PORT 8080

int main(int argc, const char* const argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char hello[] = "Hello from server";

    // create the socket file descriptor
    /*
        the socket function sets the socket protocol
        int sockfd = socket(domain, type, protocol)
        sockfd is an integer (like a file handle)
        domain is an integer which specifies the communication domain
        type is an integer which deifnes whether or not we are using TCP or UDP
            TCP = SOCK_STREAM
            UDP = SOCK_DGRAM
        protocol is the integer which appears in the protocol field of the packet
            0 is for IP
    */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Server FD: %i\n", server_fd);
    // Attach socket to port 
    /*
        int setsockopt(int sockfd, int level, int optname, const void* optval, sockelen_t optlen)
        Helps update the options referred to by the socket file descriptor sockfd
        SO_REUSEADDR allows us to reuse ports provided the addresses are diferent
        It also allows us to rebind locations in the event of the previous bound socket
        being in the TIME_WAIT state of TCP disconnect
        https://stackoverflow.com/questions/14388706/how-do-so-reuseaddr-and-so-reuseport-differ
    */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); // place the port int in network byte order

    // bind the socket to the requested source address and port
    /*
        int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
    */
    if (bind(server_fd, (struct sockaddr*) &address, (socklen_t) addrlen) < 0) {
        perror("bind failed");
        if (errno == EADDRINUSE) {
            fprintf(stderr, "EADDRINUSE\n");
        }
        else if (errno == EADDRNOTAVAIL) {
            fprintf(stderr, "EADDRNOTAVAIL\n");
        }
        else if (errno == EAFNOSUPPORT) {
            fprintf(stderr, "EAFNOSUPPORT\n");
        }
        else if (errno == EBADF) {
            fprintf(stderr, "EBADF\n");
        }
        else if (errno == EINVAL) {
            fprintf(stderr, "EINVAL\n");
        }
        else if(errno == ENOTSOCK) {
            fprintf(stderr, "ENOTSOCK\n");
        }
        else if (errno == EOPNOTSUPP) {
            fprintf(stderr, "EOPNOTSUPP\n");
        }
        fprintf(stderr, "%i\n", errno);
        exit(EXIT_FAILURE);
    }

    // place the server in listen mode
    /*
        int listen(int sockfd, int backlog)
        Puts the server socket in a passive mode where it waits for the client
        to approach the server to make a connection. The backlog defines the 
        maximum length of which the queue of pending connections for sockfd may grow.
        If a connection request arrives when the queue is full, the client may receive
        an error with an ECONNREFUSED code.
    */
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);

    return 0;
}