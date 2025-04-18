#include "storage_server.h"

struct my_struct *curr_files = NULL;
char home_dir[500];

void info_ss1(int client_socket, char *path2)
{
    struct stat file_stat;
    char *path = (char *)malloc(sizeof(char) * 100);
    strcpy(path, ".");
    strcat(path, path2);
    path[strlen(path)] = '\0';
    printf("Path: %s\n", path);
    memset(&file_stat, 0, sizeof(file_stat));
    if (stat(path, &file_stat) < 0)
    {
        perror("Error in stat() function call: ");
        int ret_val = FILE_NOT_FOUND;
        printf("Error");
        if (send(client_socket, &ret_val, sizeof(ret_val), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        close(client_socket);
        return;
    }
    else
    {
        int ret_val = SUCCESS;
        if (send(client_socket, &ret_val, sizeof(ret_val), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        usleep(1000);
        if (send(client_socket, &file_stat, sizeof(file_stat), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
    }
}

int get_random_port_number()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(0);
    server_address.sin_addr.s_addr = inet_addr(SS_IP);

    int bind_success = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_success == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }

    struct sockaddr_in temp;
    int len = sizeof(temp);
    getsockname(sock, (struct sockaddr *)&temp, (socklen_t *)&len);
    int port_number = ntohs(temp.sin_port);
    close(sock);
    return port_number;
}

int nm_conn_port;
int s2s_conn_port;
int client_conn_port;

void *check_alive(void *)
{
    int alive_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (alive_sock == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }

    struct sockaddr_in alive_server_address;
    memset(&alive_server_address, 0, sizeof(alive_server_address));
    alive_server_address.sin_family = AF_INET;
    alive_server_address.sin_port = htons(CHECK_ALIVE_PORT);
    alive_server_address.sin_addr.s_addr = inet_addr(NM_CONN_IP);

    int alive_connect_success = connect(alive_sock, (struct sockaddr *)&alive_server_address, sizeof(alive_server_address));
    if (alive_connect_success == -1)
    {
        perror("Error in connect() function call: ");
        exit(1);
    }
    while (1)
    {
        char *msg = (char *)malloc(sizeof(char) * 100);
        memset(msg, 0, 100);
        strcpy(msg, "ping");
        int bytes_sent = send(alive_sock, msg, 100, 0);
        if (bytes_sent == -1)
        {
            perror("Error in send() function call: ");
            return NULL;
        }
        sleep(1);
    }
}

void *naming_server_init(void *)
{
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }
    int server_port_number = NM_INIT_PORT;
    char *ip_address = NM_CONN_IP;
    char buffer[4096];
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port_number);
    server_address.sin_addr.s_addr = inet_addr(ip_address);

    int connect_success = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connect_success == -1)
    {
        perror("Error in connect() function call: ");
        exit(1);
    }

    struct data_of_ss initial_data_of_ss;
    initial_data_of_ss.port_number = get_random_port_number();
    initial_data_of_ss.client_port = get_random_port_number();
    initial_data_of_ss.s2s_port = get_random_port_number();
    s2s_conn_port = initial_data_of_ss.s2s_port;
    nm_conn_port = initial_data_of_ss.port_number;
    client_conn_port = initial_data_of_ss.client_port;
    printf("Enter number of paths you want to add as accessible to clients: ");
    scanf("%d", &initial_data_of_ss.number_of_paths);
    initial_data_of_ss.paths[initial_data_of_ss.number_of_paths];
    printf("Enter the paths (relative to current directory) you want to add as accessible to clients: \n");
    printf("Make sure that path starts with \"/\" and does not end with \"/\"\n");
    for (int i = 0; i < initial_data_of_ss.number_of_paths; i++)
    {
        initial_data_of_ss.paths[i].permissions = 1; // hard coded for time being . Will change this later
        printf("Enter path %d: ", i + 1);
        scanf("%s", initial_data_of_ss.paths[i].path);
        initial_data_of_ss.paths[i].path[strlen(initial_data_of_ss.paths[i].path)] = '\0';
    }

    int current_index = initial_data_of_ss.number_of_paths;
    // printf("Here151\n");
    for (int i = 0; i < initial_data_of_ss.number_of_paths; i++)
    {
        initial_data_of_ss.paths[i].backup_pending[0] = 1;
        initial_data_of_ss.paths[i].backup_pending[1] = 1;
        initial_data_of_ss.paths[i].recovery_pending[0] = 0;
        initial_data_of_ss.paths[i].recovery_pending[1] = 0;
        struct stat path_stat;
        char path2[100];
        path2[0] = '.';
        strcpy(path2 + 1, initial_data_of_ss.paths[i].path);
        path2[strlen(path2)] = '\0';
        stat(path2, &path_stat);
        if (S_ISDIR(path_stat.st_mode))
        {
            initial_data_of_ss.paths[i].dir_or_file = 1;
            DIR *dir;
            struct dirent *curr_elem_of_dir;
            if ((dir = opendir(path2)) != NULL)
            {
                curr_elem_of_dir = readdir(dir);
                while (curr_elem_of_dir != NULL)
                {
                    if (strcmp(curr_elem_of_dir->d_name, ".") != 0 && strcmp(curr_elem_of_dir->d_name, "..") != 0)
                    {
                        strcpy(initial_data_of_ss.paths[current_index].path, initial_data_of_ss.paths[i].path);
                        strcat(initial_data_of_ss.paths[current_index].path, "/");
                        strcat(initial_data_of_ss.paths[current_index].path, curr_elem_of_dir->d_name);
                        initial_data_of_ss.paths[current_index].permissions = 1;
                        current_index++;
                        initial_data_of_ss.number_of_paths++;
                    }
                    curr_elem_of_dir = readdir(dir);
                }
            }
        }
        else
        {
            struct my_struct *s;
            s = (struct my_struct *)malloc(sizeof(struct my_struct));
            strcpy(s->name, initial_data_of_ss.paths[i].path);
            s->name[strlen(s->name)] = '\0';
            s->being_written = 0;
            pthread_mutex_init(&s->mutex, NULL);
            HASH_ADD_STR(curr_files, name, s);
            initial_data_of_ss.paths[i].dir_or_file = 0;
            // printf("Added %s to hash table\n", s->name);
            struct my_struct *s1;
            HASH_FIND_STR(curr_files, s->name, s1);
            // if (s1==NULL)
            // {
            //     printf("Not added\n");
            // }
            // else
            // {
            //     printf("Added\n");
            // }
        }
    }

    int err_check = send(sock, &initial_data_of_ss, sizeof(initial_data_of_ss), 0);
    if (err_check == -1)
    {
        perror("Error in send() function call: ");
        return NULL;
    }
    printf(GREEN "[NAMING SERVER] Sent initial data to naming server\n" RESET);

    pthread_t tid;
    pthread_create(&tid, NULL, check_alive, NULL);
    struct my_struct *s;
    // for(s=curr_files;s!=NULL;s=s->hh.next)
    // {
    //     printf("Name: %s\n",s->name);
    // }
    // close(sock);
    return NULL;
}

void *naming_server_communication(void *)
{
    int socket_nm = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_nm == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }

    struct sockaddr_in server_address_nm;

    memset(&server_address_nm, 0, sizeof(server_address_nm));
    server_address_nm.sin_family = AF_INET;
    server_address_nm.sin_port = htons(nm_conn_port);
    server_address_nm.sin_addr.s_addr = inet_addr(SS_IP);

    if (bind(socket_nm, (struct sockaddr *)&server_address_nm, sizeof(server_address_nm)) == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }

    if (listen(socket_nm, 20) == -1)
    {
        perror("Error in listen() function call: ");
        exit(1);
    }

    while (1)
    {
        int client_socket_nm;
        struct sockaddr_in client_address_nm;
        int client_address_length_nm = sizeof(client_address_nm);
        client_socket_nm = accept(socket_nm, (struct sockaddr *)&client_address_nm, &client_address_length_nm);
        if (client_socket_nm == -1)
        {
            perror("Error in accept() function call: ");
            exit(1);
        }

        char *buffer1 = (char *)malloc(4096);
        memset(buffer1, 0, 4096);
        int bytes_received = recv(client_socket_nm, buffer1, 4096, 0);
        if (bytes_received == -1)
        {
            perror("Error in recv() function call: ");
            exit(1);
        }
        char *token = strtok(buffer1, " ");

        if (strcmp(token, "makedir") == 0)
        {
            char *dirname = strtok(NULL, " ");
            char *path = strtok(NULL, " ");
            makedirss(dirname, path, client_socket_nm);
        }
        else if (strcmp(token, "deletedir") == 0)
        {
            char *path = strtok(NULL, " ");
            deletedirss(path, client_socket_nm);
        }
        else if (strcmp(token, "makefile") == 0)
        {
            char *filename = strtok(NULL, " ");
            char *path = strtok(NULL, " ");
            makefiless(filename, path, client_socket_nm);
        }
        else if (strcmp(token, "deletefile") == 0)
        {
            char *path = strtok(NULL, " ");
            deletefiless(path, client_socket_nm);
        }
        else if (strcmp(token, "copyfile") == 0)
        {
            char *temp = strtok(NULL, " ");
            if (strcmp(temp, "dest") == 0)
            {
                char *dest = strtok(NULL, " ");
                ss_info *ss_to_receive = (ss_info *)malloc(sizeof(ss_info));
                int bytes_received = recv(client_socket_nm, ss_to_receive, sizeof(ss_info), 0);
                if (bytes_received == -1)
                {
                    perror("Error in recv() function call: ");
                    exit(1);
                }

                copyfilereceive(dest, ss_to_receive, client_socket_nm);
            }
            else if (strcmp(temp, "src") == 0)
            {
                char *src = strtok(NULL, " ");
                ss_info *ss_to_send = (ss_info *)malloc(sizeof(ss_info));
                int bytes_received = recv(client_socket_nm, ss_to_send, sizeof(ss_info), 0);
                if (bytes_received == -1)
                {
                    perror("Error in recv() function call: ");
                    exit(1);
                }

                copyfiless(src, ss_to_send, client_socket_nm, s2s_conn_port);
            }
            else if (strcmp(temp, "same") == 0)
            {
                char *src = strtok(NULL, " ");
                char *dest = strtok(NULL, " ");
                copyfiless_same(src, dest, client_socket_nm, s2s_conn_port);
            }
        }
        else if (strcmp(token, "copydir") == 0)
        {

            char *temp = strtok(NULL, " ");
            if (strcmp(temp, "dest") == 0)
            {
                char *dest = strtok(NULL, " ");
                ss_info *ss_to_receive = (ss_info *)malloc(sizeof(ss_info));
                int bytes_received = recv(client_socket_nm, ss_to_receive, sizeof(ss_info), 0);
                if (bytes_received == -1)
                {
                    perror("Error in recv() function call: ");
                    exit(1);
                }
                // printf("Received\n");
                copydirreceive(dest, ss_to_receive, client_socket_nm);
            }
            else if (strcmp(temp, "src") == 0)
            {
                char *src = strtok(NULL, " ");
                ss_info *ss_to_send = (ss_info *)malloc(sizeof(ss_info));
                int bytes_received = recv(client_socket_nm, ss_to_send, sizeof(ss_info), 0);
                if (bytes_received == -1)
                {
                    perror("Error in recv() function call: ");
                    exit(1);
                }
                // printf("Received\n");
                copydirss(src, ss_to_send, client_socket_nm, s2s_conn_port);
            }
            else if (strcmp(temp, "same") == 0)
            {
                char *src = strtok(NULL, " ");
                char *dest = strtok(NULL, " ");
                copydirss_same(src, dest, client_socket_nm, s2s_conn_port);
            }
        }

        close(client_socket_nm);
    }
}

void *client_req_handler(void *arg)
{
    pthread_detach(pthread_self());
    int client_socket = *((int *)arg);
    char *msg = (char *)malloc(sizeof(char) * 100);
    memset(msg, 0, 100);
    if (recv(client_socket, msg, 100, 0) < 0)
    {
        perror("Error in recv() function call: ");
        exit(1);
    }
    char *token = strtok(msg, " ");
    if (strcmp(token, "read") == 0)
    {
        char *path = strtok(NULL, " ");
        readss(path, client_socket);
    }
    else if (strcmp(token, "write") == 0)
    {
        char *path = strtok(NULL, " ");
        char *data = strtok(NULL, "\"");
        writess(path, data, client_socket);
    }
    else if (strcmp(token, "info") == 0)
    {
        /* code */
        char *path = strtok(NULL, " ");
        // info_ss1(client_socket, path);
        info_ss1(client_socket, path);
    }

    close(client_socket);
    return NULL;
}

int main()
{
    printf("Welcome to storage server\n");
    printf("Please follow the instructions to start the storage server \n");

    getcwd(home_dir, 500);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, naming_server_init, NULL);

    pthread_join(thread_id, NULL);

    pthread_t thread_id2;
    pthread_create(&thread_id2, NULL, naming_server_communication, NULL);

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }

    struct sockaddr_in server_address_client;

    memset(&server_address_client, 0, sizeof(server_address_client));
    server_address_client.sin_family = AF_INET;
    server_address_client.sin_port = htons(client_conn_port);
    server_address_client.sin_addr.s_addr = inet_addr(SS_IP);

    if (bind(socket_client, (struct sockaddr *)&server_address_client, sizeof(server_address_client)) == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }

    if (listen(socket_client, 10) == -1)
    {
        perror("Error in listen() function call: ");
        exit(1);
    }

    while (1)
    {
        int client_socket_client;
        struct sockaddr_in client_address_client;
        int client_address_length_client = sizeof(client_address_client);
        client_socket_client = accept(socket_client, (struct sockaddr *)&client_address_client, &client_address_length_client);
        if (client_socket_client == -1)
        {
            perror("Error in accept() function call: ");
            exit(1);
        }
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_req_handler, &client_socket_client);
    }

    pthread_join(thread_id2, NULL);
    return 0;
}