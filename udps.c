#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define PORT 8889

void die(const char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

void receive_file(int sockfd, const char *file_type) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char buffer[MAX_BUFFER_SIZE];
    ssize_t recv_bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);

    if (recv_bytes == -1) {
        die("Error receiving data");
    }

    char file_name[MAX_BUFFER_SIZE];
    snprintf(file_name, sizeof(file_name), "%s_received", file_type);

    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    fwrite(buffer + strlen(file_type) + 1, 1, recv_bytes - strlen(file_type) - 1, file);
    fclose(file);

    printf("%s file received successfully.\n", file_type);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        die("Error creating socket");
    }

    // Set the socket option to reuse the address
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        die("Error setting socket options");
    }

    struct sockaddr_in server_addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(PORT)};
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        die("Error binding socket");
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        receive_file(sockfd, "text");
        receive_file(sockfd, "audio");
        receive_file(sockfd, "video");
    }

    close(sockfd);
    return 0;
}

