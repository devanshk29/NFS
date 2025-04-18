
void *send_heartbeat()
{
     char *ip = NM_CONN_IP;
  int port = Heartbeat_PORT;
  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
//    printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);
  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
// printf("Connected to the server.\n");

    while (1)
    {
        char *heartbeat = "HEARTBEAT";
        if (send(sock, heartbeat, strlen(heartbeat), 0) < 0)
        {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
// printf("Heartbeat sent\n");
        sleep(5); // Send a heartbeat every 5 seconds
    }

    return NULL;
}


int main()
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, naming_server_init, NULL);

    pthread_join(thread_id, NULL);

    pthread_t thread_id2;
    pthread_create(&thread_id2, NULL, naming_server_communication, NULL);

    pthread_t thread_id3;
    pthread_create(&thread_id3, NULL, client_thread, NULL);

    pthread_t heartbeat_thread;
    pthread_create(&heartbeat_thread, NULL, send_heartbeat, NULL);

    pthread_join(thread_id2, NULL);
    pthread_join(thread_id3, NULL);
    pthread_join(heartbeat_thread, NULL);
    return 0;
}