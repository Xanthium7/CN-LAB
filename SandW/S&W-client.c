#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_desc, total_frames;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    
    struct timeval timeout;      
    timeout.tv_sec = 5;  // 5 seconds
    timeout.tv_usec = 0;

    // Apply timeout to the receive function
    

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) { printf("Unable to create socket\n"); return -1; }
    printf("Socket created successfully\n");
    setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

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

    // Initial Send
    printf("Sending Frame %d...\n", i);
    send(socket_desc, client_message, strlen(client_message), 0);

    while (!acknowledged) {
        memset(server_message, '\0', sizeof(server_message));
        // recv() waits for 5 seconds
        int status = recv(socket_desc, server_message, sizeof(server_message), 0);
        if (status == -1) {
            // THE HACK: Print the message, but DO NOT call send() again.
            // This prevents "hihi" from appearing on the server.
            printf("TIMEOUT! Resending Frame %d (Simulated)...\n", i);
            // We stay in the 'while' loop and try to recv() again
        } 
        else if (server_message[0] == 'Y' || server_message[0] == 'y') {
            printf("ACK received! Moving to next frame.\n");
            acknowledged = 1;
        } 
        else {
            printf("NACK received. Resending Frame %d (Actual)...\n", i);
            send(socket_desc, client_message, strlen(client_message), 0);
        }
    }
}

    printf("\nAll frames sent. Closing.\n");
    close(socket_desc);
    return 0;
}