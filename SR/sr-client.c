#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h> // ADDED: For timeout structures

#define MAX 80
#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buff[MAX];
    
    int nf, ws, w1 = 1, w2;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) != 0) {
        printf("Connection failed.\n");
        return 1;
    }
    
    // ADDED: Set a 3-second receive timeout on the socket
    struct timeval tv;
    tv.tv_sec = 6;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    
    printf("Connected to server.\n");
    printf("Total frames to send: ");
    scanf("%d", &nf);
    printf("Window size: ");
    scanf("%d", &ws);
    
    w2 = w1 + ws - 1;
    if (w2 > nf) {
        w2 = nf; 
    }
    
    // Send the initial batch
    for (int i = w1; i <= w2; i++) {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%d", i);
        send(sockfd, buff, sizeof(buff), 0);
        printf("\nFrame %d sent", i);
    }

    // Main listening loop
    while (1) {
        memset(buff, '\0', sizeof(buff));
        
        // ADDED: Capture return value of recv to check for timeouts
        int bytes_read = recv(sockfd, buff, sizeof(buff), 0);
        
        if (bytes_read == 0) {
            printf("\nServer disconnected.\n");
            break;
        } 
        else if (bytes_read < 0) {
            // ADDED: Timeout triggered! Resend current window.
            printf("\n\n[TIMEOUT] No response for 3 seconds. Resending window (%d to %d)...", w1, w2);
            for (int i = w1; i <= w2; i++) {
                memset(buff, 0, sizeof(buff));
                sprintf(buff, "%d", i);
                send(sockfd, buff, sizeof(buff), 0);
                printf("\nFrame %d resent", i);
            }
            continue; // Skip the rest of the loop and wait for new ACKs
        }
        
        int ack = atoi(buff);

        if (ack < 0) { 
            // NACK received -> Resend ONLY the missing frame
            ack = -ack; 
            printf("\n[NACK] Server missed frame %d. Resending %d...", ack, ack);
            memset(buff, '\0', sizeof(buff));
            sprintf(buff, "%d", ack);
            send(sockfd, buff, sizeof(buff), 0);
        } 
        else if (ack >= nf) { 
            // Done
            printf("\nAll frames successfully acknowledged!\n");
            memset(buff, '\0', sizeof(buff));
            strcpy(buff, "EXIT");
            send(sockfd, buff, sizeof(buff), 0);
            break;
        } 
        else if (ack >= w1) {
            // Standard ACK received -> Slide window and send new frames
            printf("\n[ACK] Received %d", ack);
            w1 = ack + 1; 
            int new_w2 = w1 + ws - 1;
            
            if (new_w2 > nf) {
                new_w2 = nf;
            }
            
            for (int i = w2 + 1; i <= new_w2; i++) {
                memset(buff, 0, sizeof(buff));
                sprintf(buff, "%d", i);
                send(sockfd, buff, sizeof(buff), 0);
                printf("\nFrame %d sent", i);
            }
            w2 = new_w2; 
        }
    }

    close(sockfd);
    return 0;
}