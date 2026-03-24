#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd;
    char buffer[2000];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("UDP Server listening on port 2000...\n");

    len = sizeof(client_addr);

    // Receive
    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&client_addr, &len);

    printf("Message from client: %s\n", buffer);

    // Send reply
    strcpy(buffer, "Hello from UDP Server");

    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr*)&client_addr, len);

    close(sockfd);
    return 0;
}