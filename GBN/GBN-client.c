#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define MAX 80
#define PORT 8080

int main() {
    int sockfd, nf, ws;
    struct sockaddr_in server_addr;
    char buff[MAX];
    int ack = -1, w1 = 0, w2, i;

    // Set timeout to 3 seconds
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // CRITICAL: Enable the timeout for the recv function
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Enter number of frames: ");
    scanf("%d", &nf);
    printf("Enter window size: ");
    scanf("%d", &ws);

    w2 = ws - 1;

    while (w1 < nf) {
        // Send all frames in the current window
        for (i = w1; i <= w2 && i < nf; i++) {
            snprintf(buff, sizeof(buff), "%d", i);
            send(sockfd, buff, sizeof(buff), 0);
            printf("Sent Frame %d\n", i);
        }

        // Wait for ACKs
        while (1) {
            memset(buff, '\0', sizeof(buff));
            int n = recv(sockfd, buff, MAX, 0);

            if (n > 0) {
                ack = atoi(buff);
                printf("Received ACK %d\n", ack);

                if (ack >= w1) { // Sliding the window
                    w1 = ack + 1;
                    w2 = w1 + ws - 1;
                    if (w1 >= nf) break; // All frames acknowledged
                }
            } else {
                // n < 0 usually indicates a timeout
                printf("TIMEOUT: Resending Window [%d to %d]\n", w1, (w2 < nf-1 ? w2 : nf-1));
                break; // Break the inner loop to resend the window
            }
        }
    }

    printf("\nAll frames sent and acknowledged successfully.\n");
    strcpy(buff, "EXIT");
    send(sockfd, buff, sizeof(buff), 0);
    
    close(sockfd);
    return 0;
}