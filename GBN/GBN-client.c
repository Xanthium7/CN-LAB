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
    int next_to_send = 0;

    // 1. Setup Timeout (3 seconds)
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("\nSocket creation failed\n");
        return -1;
    }

    // Apply the timeout to the socket
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection to server failed\n");
        return -1;
    }

    printf("\n--- Go-Back-N Client Started ---\n");
    printf("Enter total number of frames to send: ");
    scanf("%d", &nf);
    printf("Enter window size: ");
    scanf("%d", &ws);

    w2 = ws - 1; // Initial window end

    while (w1 < nf) {
        // STEP A: Send all frames in the current window that haven't been sent yet
        while (next_to_send <= w2 && next_to_send < nf) {
            sprintf(buff, "%d", next_to_send);
            send(sockfd, buff, sizeof(buff), 0);
            printf("Sent Frame %d\n", next_to_send);
            next_to_send++;
        }

        // STEP B: Wait for an ACK
        memset(buff, '\0', sizeof(buff));
        int n = recv(sockfd, buff, MAX, 0);

        if (n > 0) {
            ack = atoi(buff);
            printf("Received ACK %d\n", ack);

            // If it's a valid ACK, slide the window forward
            if (ack >= w1) {
                w1 = ack + 1;
                w2 = w1 + ws - 1;
            }
        } else {
            // STEP C: Handle Timeout (The "Go-Back" trigger)
            printf("TIMEOUT: No ACK received. Resending from Frame %d\n", w1);
            next_to_send = w1; // Reset our pointer to the start of the window
        }
    }

    printf("\nAll %d frames sent and acknowledged. Closing connection.\n", nf);
    
    // Tell the server we are done
    strcpy(buff, "EXIT");
    send(sockfd, buff, sizeof(buff), 0);

    close(sockfd);
    return 0;
}