#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) { printf("Error while creating socket\n"); return -1; }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to the port\n"); return -1;
    }
    printf("Done with binding\nListening for incoming connections...\n");

    listen(socket_desc, 1);
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

    while (1) {
        memset(client_message, '\0', sizeof(client_message));
        // Wait for a frame from client
        if (recv(client_sock, client_message, sizeof(client_message), 0) <= 0) break;

        printf("\nMsg from client: %s\n", client_message);

        // Ask user for ACK or NACK
        printf("Enter Response (Y for ACK / N for NACK): ");
        scanf("%s", server_message);
        send(client_sock, server_message, strlen(server_message), 0);
    }

    close(client_sock);
    close(socket_desc);
    return 0;
}