#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define MAX 80
#define PORT 8080

void sendAck(int sockfd, int ack) {
    char buff[MAX];
    sprintf(buff, "%d", ack);
    send(sockfd, buff, sizeof(buff), 0);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    char buff[MAX];
    int f, next = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("\nSocket creation failed\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    listen(sockfd, 5);
    printf("GBN Server waiting on port %d...\n", PORT);

    len = sizeof(client_addr);
    connfd = accept(sockfd, (struct sockaddr*)&client_addr, &len);

    srand(time(NULL)); 

    while (1) {
        memset(buff, '\0', sizeof(buff));
        int n = recv(connfd, buff, MAX, 0);
        
        if (n <= 0 || strcmp(buff, "EXIT") == 0) {
            printf("\nClient disconnected.\n");
            break;
        }

        f = atoi(buff);

        // Check if frame is not the one we expect
        if (f != next) {
            printf("Frame %d discarded (Expected: %d). Sending ACK for %d\n", f, next, next - 1);
            sendAck(connfd, next - 1);
            continue;
        }

        // Simulate network unreliability
        int r = rand() % 3; 
        if (r == 0) { // Simulate Packet Loss
            printf("Frame %d was lost (Simulated)\n", f);
            continue; 
        } 

        printf("Frame %d received. Sending ACK %d\n", f, f);
        sendAck(connfd, f);
        next++;
    }

    close(connfd);
    close(sockfd);
    return 0;
}