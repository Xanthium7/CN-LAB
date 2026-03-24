#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd;
    char buffer[2000];
    struct sockaddr_in server_addr;
    socklen_t len;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Input message
    printf("Enter message: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Send
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Receive
    len = sizeof(server_addr);
    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&server_addr, &len);

    printf("Server reply: %s\n", buffer);

    close(sockfd);
    return 0;
}