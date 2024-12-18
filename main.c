#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

#define BUFFER_SIZE 4096

void send_url_to_server(const char *host, int port) {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char url[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return;
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Setup server address
    server_addr.sin_addr.s_addr = inet_addr(host);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    printf("Connected to the server at %s:%d\n", host, port);

    // Communication loop
    while (1) {
        printf("Enter the URL of the image (or type 'exit' to quit): ");
        fgets(url, BUFFER_SIZE, stdin);

        // Remove newline character from input
        size_t len = strlen(url);
        if (len > 0 && url[len - 1] == '\n') {
            url[len - 1] = '\0';
        }

        // Exit condition
        if (strcmp(url, "exit") == 0) {
            printf("Exiting the client.\n");
            break;
        }

        // Send URL to the server
        if (send(client_socket, url, strlen(url), 0) < 0) {
            printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
            break;
        }

        // Receive response from the server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            printf("Failed to receive data. Error Code: %d\n", WSAGetLastError());
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Server response:\n%s\n", buffer);
    }

    // Close the socket and cleanup Winsock
    closesocket(client_socket);
    WSACleanup();
}

int main() {
    const char *host = "127.0.0.1";
    int port = 65432;

    send_url_to_server(host, port);
    return 0;
}
