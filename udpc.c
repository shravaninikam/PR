#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 8889

void die(const char *error) {
    perror(error);
    exit(EXIT_FAILURE);
}

void send_file(int sockfd, const char *file_path, const char *file_type, struct sockaddr_in server_addr) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        die("Error opening file for reading");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[MAX_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s", file_type);
    fread(buffer + strlen(file_type) + 1, 1, sizeof(buffer) - strlen(file_type) - 1, file);

    if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        die("Error sending data");
    }

    fclose(file);
    printf("%s file sent successfully.\n", file_type);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        die("Error creating socket");
    }

    struct sockaddr_in server_addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        die("Error converting IP address");
    }

    send_file(sockfd, "/home/student/text_file.txt", "text", server_addr);
    send_file(sockfd, "/home/student/audio_file.wav", "audio", server_addr);
    send_file(sockfd, "/home/student/video_file.mp4", "video", server_addr);

    close(sockfd);
    return 0;
}

