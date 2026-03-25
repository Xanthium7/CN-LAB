#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX 80
#define PORT 8080

int main() {
    int sockfd, connfd, len;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAX];
    
    int ws, w1 = 1, f;
    int flag[200] = {0}; // Buffer for received frames
    int last_nack = 0;   // ADDED: Tracks the last NACK sent to prevent spam

    srand(time(NULL));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(sockfd, 5);
    
    printf("Server listening...\n");
    len = sizeof(clientaddr);
    connfd = accept(sockfd, (struct sockaddr*)&clientaddr, (socklen_t*)&len);
    
    printf("Client connected! Give window size: ");
    scanf("%d", &ws);

    while (1) {
        sleep(1); 
        memset(buff, '\0', sizeof(buff));
        int bytes_read = recv(connfd, buff, sizeof(buff), 0);
        if (bytes_read <= 0) {
            printf("\nClient disconnected.\n");
            break;
        }
        if (strcmp(buff, "EXIT") == 0) break;
        f = atoi(buff);

        if (rand() % 4 == 0) {
            printf("\n[X] Frame %d lost", f);
            continue;
        }

        if (f == w1) {
            printf("\n[+] Frame %d received in order", f);
            flag[f] = 1;
            
            while (flag[w1] == 1) {
                w1++;
            }
            // ADDED: Reset NACK tracker because the window moved forward
            last_nack = 0; 
            // Send standard ACK
            memset(buff, '\0', sizeof(buff));
            sprintf(buff, "%d", w1 - 1);
            send(connfd, buff, sizeof(buff), 0);
            
        } else if (f > w1 && flag[f] == 0) {
            // Out-of-order frame received
            flag[f] = 1; // Buffer it
            
            // ADDED: NACK Throttling - Only send NACK if we haven't asked for it already
            if (last_nack != w1) {
                printf("\n[!] Frame %d received out of order. Buffering and sending NACK %d", f, w1);
                
                memset(buff, '\0', sizeof(buff));
                sprintf(buff, "%d", -1 * w1);
                send(connfd, buff, sizeof(buff), 0);
                
                last_nack = w1; // Remember we asked for this
            } else {
                printf("\n[!] Frame %d received out of order. Quietly buffering...", f);
            }
        }
    }

    close(connfd);
    close(sockfd);
    printf("\nTransmission complete.\n");
    return 0;
}