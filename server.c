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


int main(int argc, char** argv) {
    ServerConfig config;
    parse_arguments(argc, argv, &config);
    start_server(&config);
    return 0;
}