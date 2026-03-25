#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_desc, total_frames;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) { printf("Unable to create socket\n"); return -1; }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n"); return -1;
    }
    printf("Connected with server successfully\n");

    printf("Enter total frames: ");
    scanf("%d", &total_frames);

    for (int i = 1; i <= total_frames; i++) {
        int acknowledged = 0; 
        printf("\nEnter message for Frame %d: ", i);
        scanf("%s", client_message);

        // This loop handles the "Stop and Wait" logic
        while (!acknowledged) {
            printf("Sending Frame %d...\n", i);
            send(socket_desc, client_message, strlen(client_message), 0);
            
            memset(server_message, '\0', sizeof(server_message));
            recv(socket_desc, server_message, sizeof(server_message), 0);

            if (server_message[0] == 'Y' || server_message[0] == 'y') {
                printf("ACK received! Moving to next frame.\n");
                acknowledged = 1; // Success! This exits the 'while' and increments 'i'
            } else {
                printf("NACK received. Resending Frame %d...\n", i);
                // acknowledged remains 0, so the 'while' loop runs again
            }
        }
    }

    printf("\nAll frames sent. Closing.\n");
    close(socket_desc);
    return 0;
}