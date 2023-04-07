#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "server_function.h"

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 80
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PATH "./"



void print_usage() {
    printf("Usage: ./server [-a ADDRESS] [-p PORT] [-d PATH]\n");
    printf("\tADDRESS: server address (default: 127.0.0.1)\n");
    printf("\tPORT: server port (default: 80)\n");
    printf("\tPATH: file system path for html files (default: ./)\n");
}

void parse_arguments(int argc, char** argv, ServerConfig* config) {
    config->address = DEFAULT_ADDRESS;
    config->port = DEFAULT_PORT;
    config->path = DEFAULT_PATH;

    int opt;
    while ((opt = getopt(argc, argv, "a:p:d:")) != -1) {
        switch (opt) {
            case 'a':
                config->address = optarg;
                break;
            case 'p':
                config->port = atoi(optarg);
                break;
            case 'd':
                config->path = optarg;
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }
}

bool is_html_file(char* filename) {
    char* extension = strrchr(filename, '.');
    if (extension == NULL) {
        return false;
    }
    return strcmp(extension, ".html") == 0;
}

void handle_client(int client_socket, char* filename, char* path) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s%s", path, filename);

    if (!is_html_file(filename)) {
        char* response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        write(client_socket, response, strlen(response));
        printf("Request for %s: Invalid file extension\n", filename);
        return;
    }

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        char* response = "HTTP/1.1 404 Not Found\r\n\r\n";
        write(client_socket, response, strlen(response));
        printf("Request for %s: File not found\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* response = (char*) malloc(BUFFER_SIZE + file_size);
    snprintf(response, BUFFER_SIZE + file_size, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", file_size);
    fread(response + strlen(response), file_size, 1, file);

    write(client_socket, response, strlen(response));
    printf("Request for %s: File found and served\n", filename);

    free(response);
    fclose(file);
}


void start_server(ServerConfig* config) {
    int server_socket;
    struct sockaddr_in server_address;
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_length;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(config->address);
    server_address.sin_port = htons(config->port);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started on %s:%d\n", config->address, config->port);

    while (true) {
        client_address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_address_length);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        char request[BUFFER_SIZE];
        ssize_t request_size = read(client_socket, request, BUFFER_SIZE);
        if (request_size < 0) {
            perror("read");
            continue;
        }

        char method[10], filename[BUFFER_SIZE], protocol[10];
        sscanf(request, "%s %s %s", method, filename, protocol);

        handle_client(client_socket, filename, config -> path);

        close(client_socket);
    }

    close(server_socket);
}
