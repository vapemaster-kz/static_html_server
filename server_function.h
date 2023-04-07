// server_function.h
typedef struct {
    char* address;
    int port;
    char* path;
} ServerConfig;

void print_usage();

void parse_arguments(int argc, char** argv, ServerConfig* config);

bool is_html_file(char* filename);

void handle_client(int client_socket, char* filename, char* path);

void start_server(ServerConfig* config);

