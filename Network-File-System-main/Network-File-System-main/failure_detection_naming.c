
void *listen_heartbeat() {
    char *ip = IP;
    int port = Heartbeat_PORT;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[-]Bind error");
        exit(1);
    }

    listen(server_sock, 5);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);

        if (client_sock < 0) {
            perror("[-]Accept error");
            exit(1);
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &client_sock);
    }

    return NULL;
}

int main()
{
    pthread_t ss_init_thread_id;
    pthread_create(&ss_init_thread_id, NULL, ss_init_thread, NULL);
    pthread_t client_thread_id;
    pthread_create(&client_thread_id, NULL, client_thread, NULL);
     pthread_t heartbeat_thread;
    pthread_create(&heartbeat_thread, NULL, listen_heartbeat, NULL);

    pthread_join(ss_init_thread_id, NULL);
    pthread_join(heartbeat_thread, NULL);
    return 0;
}