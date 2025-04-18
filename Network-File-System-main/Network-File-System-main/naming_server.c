#include "naming_server.h"
#include "storage_server.h"

// 1 for dir and 2 for file
struct tree_node *root;

int failure[100000];
struct data_of_ss initial_data[100];
int backup_pending[1000];
int recovery_pending[1000];
int curr_number_of_ss;

struct replica_info backup_arr[100];

struct arg_struct
{
    int client_socket;
    int ss_number;
};

pthread_mutex_t mutex;
pthread_mutex_t init_lock;

void *ss_backup_update(void *)
{
    pthread_detach(pthread_self());
    // int ss_number = *((int *)arg);
    while (1)
    {
        for (int ss_number = 0; ss_number < 100; ss_number++)
        {
            if (backup_pending[ss_number] == 1 && failure[ss_number] == 0)
            {
                // int ss_success = 1;
                int ss_success_count = 0;
                int ctr = 0;
                // pthread_mutex_lock(&mutex);
                // usleep(5000000);
                for (int i = 0; i < initial_data[ss_number].number_of_paths; i++)
                {

                    if (initial_data[ss_number].paths[i].backup_pending[0] == 0 && initial_data[ss_number].paths[i].backup_pending[1] == 0)
                    {
                        continue;
                    }
                    else
                    {
                        ctr++;
                    }
                    if (backup_arr[ss_number].replica1_ss_index != -1 && initial_data[ss_number].paths[i].backup_pending[0] == 1)
                    {

                        ss_info sender, receiver;
                        sender.ss_port = initial_data[ss_number].port_number;
                        sender.s2s_port = initial_data[ss_number].s2s_port;
                        sender.dir_or_file = 1;
                        strcpy(sender.ss_ip, initial_data[ss_number].ip);
                        sender.ss_ip[strlen(sender.ss_ip)] = '\0';
                        int index1 = backup_arr[ss_number].replica1_ss_index;
                        receiver.ss_port = initial_data[index1].port_number;
                        receiver.s2s_port = initial_data[index1].s2s_port;
                        receiver.dir_or_file = 1;
                        strcpy(receiver.ss_ip, initial_data[index1].ip);
                        receiver.ss_ip[strlen(receiver.ss_ip)] = '\0';
                        if (initial_data[ss_number].paths[i].dir_or_file == 1)
                        {
                            backupdir(sender, receiver, initial_data[ss_number].paths[i].path);
                            /* code */
                        }
                        else if (initial_data[ss_number].paths[i].dir_or_file == 0)
                        {
                            /* code */
                            backup_file(sender, receiver, initial_data[ss_number].paths[i].path);
                        }
                        initial_data[ss_number].paths[i].backup_pending[0] = 0;
                    }

                    if (backup_arr[ss_number].replica2_ss_index != -1 && initial_data[ss_number].paths[i].backup_pending[1] == 1)
                    {
                        ss_info sender, receiver;
                        sender.ss_port = initial_data[ss_number].port_number;
                        sender.s2s_port = initial_data[ss_number].s2s_port;
                        sender.dir_or_file = 1;
                        strcpy(sender.ss_ip, initial_data[ss_number].ip);
                        sender.ss_ip[strlen(sender.ss_ip)] = '\0';
                        int index1 = backup_arr[ss_number].replica2_ss_index;
                        receiver.ss_port = initial_data[index1].port_number;
                        receiver.s2s_port = initial_data[index1].s2s_port;
                        receiver.dir_or_file = 1;
                        strcpy(receiver.ss_ip, initial_data[index1].ip);
                        receiver.ss_ip[strlen(receiver.ss_ip)] = '\0';
                        if (initial_data[ss_number].paths[i].dir_or_file == 1)
                        {
                            backupdir(sender, receiver, initial_data[ss_number].paths[i].path);
                            /* code */
                        }
                        else if (initial_data[ss_number].paths[i].dir_or_file == 0)
                        {
                            /* code */
                            backup_file(sender, receiver, initial_data[ss_number].paths[i].path);
                        }
                        initial_data[ss_number].paths[i].backup_pending[1] = 0;
                    }
                }
                if (ctr == 0)
                {
                    backup_pending[ss_number] = 0;
                }
            }

            if (recovery_pending[ss_number] == 1 && failure[ss_number] == 0)
            {
                int ctr2 = 0;
                for (int i = 0; i < initial_data[ss_number].number_of_paths; i++)
                {
                    if (initial_data[ss_number].paths[i].recovery_pending[0] == 0 && initial_data[ss_number].paths[i].recovery_pending[1] == 0)
                    {
                        continue;
                    }
                    else
                    {
                        ctr2++;
                    }

                    if (initial_data[ss_number].paths[i].recovery_pending[0] == 1)
                    {
                        ss_info sender, receiver;
                        int recv_idx = backup_arr[ss_number].replica1_ss_index;
                        sender.ss_port = initial_data[recv_idx].port_number;
                        sender.s2s_port = initial_data[recv_idx].s2s_port;
                        sender.dir_or_file = 1;
                        strcpy(sender.ss_ip, initial_data[recv_idx].ip);
                        sender.ss_ip[strlen(sender.ss_ip)] = '\0';
                        int index1 = ss_number;
                        receiver.ss_port = initial_data[index1].port_number;
                        receiver.s2s_port = initial_data[index1].s2s_port;
                        receiver.dir_or_file = 1;
                        strcpy(receiver.ss_ip, initial_data[index1].ip);
                        receiver.ss_ip[strlen(receiver.ss_ip)] = '\0';
                        if (initial_data[ss_number].paths[i].dir_or_file == 1)
                        {
                            backupdir(sender, receiver, initial_data[ss_number].paths[i].path);
                        }
                        else if (initial_data[ss_number].paths[i].dir_or_file == 0)
                        {
                            backup_file(sender, receiver, initial_data[ss_number].paths[i].path);
                        }

                        initial_data[ss_number].paths[i].recovery_pending[0] = 0;
                        backup_pending[ss_number] = 1;
                        initial_data[ss_number].paths[i].backup_pending[1] = 1;
                    }

                    if (initial_data[ss_number].paths[i].recovery_pending[1] == 1)
                    {
                        ss_info sender, receiver;
                        int recv_idx = backup_arr[ss_number].replica2_ss_index;
                        sender.ss_port = initial_data[recv_idx].port_number;
                        sender.s2s_port = initial_data[recv_idx].s2s_port;
                        sender.dir_or_file = 1;
                        strcpy(sender.ss_ip, initial_data[recv_idx].ip);
                        sender.ss_ip[strlen(sender.ss_ip)] = '\0';
                        int index1 = ss_number;
                        receiver.ss_port = initial_data[index1].port_number;
                        receiver.s2s_port = initial_data[index1].s2s_port;
                        receiver.dir_or_file = 1;
                        strcpy(receiver.ss_ip, initial_data[index1].ip);
                        receiver.ss_ip[strlen(receiver.ss_ip)] = '\0';
                        if (initial_data[ss_number].paths[i].dir_or_file == 1)
                        {
                            backupdir(sender, receiver, initial_data[ss_number].paths[i].path);
                        }
                        else if (initial_data[ss_number].paths[i].dir_or_file == 0)
                        {
                            backup_file(sender, receiver, initial_data[ss_number].paths[i].path);
                        }
                        initial_data[ss_number].paths[i].recovery_pending[1] = 0;
                        backup_pending[ss_number] = 1;
                        initial_data[ss_number].paths[i].backup_pending[0] = 1;
                    }
                }
                if (ctr2 == 0)
                {
                    recovery_pending[ss_number] = 0;
                }
            }
        }
    }
}

int alive_socket;

sem_t sem[1000];

void *checkalive(void *arg)
{
    pthread_detach(pthread_self());
    int ss_number = *((int *)arg);
    sem_wait(&sem[ss_number]);
    int client_socket;
    struct sockaddr_in client_address;
    int client_address_length = sizeof(client_address);
    client_socket = accept(alive_socket, (struct sockaddr *)&client_address, &client_address_length);
    if (client_socket == -1)
    {
        perror("Error in accept() function call: ");
        exit(1);
    }
    sem_post(&sem[ss_number + 1]);

    int prev_failure = 0;

    while (1)
    {
        char *msg = (char *)malloc(sizeof(char) * 100);
        memset(msg, 0, 100);
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
        int recv_success = recv(client_socket, msg, 100, 0);
        if (recv_success == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                if (prev_failure == 0)
                {
                    printf(RED "Storage server with ip %s and port %d is dead\n" RESET, initial_data[ss_number].ip, initial_data[ss_number].port_number);
                }
                failure[ss_number] = 1;
                prev_failure = 1;
                sleep(1);
                continue;
            }
            else
            {
                perror("Error in recv() function call: ");
                return NULL;
            }
        }
        else if (recv_success == 0)
        {
            if (prev_failure == 0)
            {
                printf(RED "Storage server with ip %s and port %d is dead\n" RESET, initial_data[ss_number].ip, initial_data[ss_number].port_number);
            }
            failure[ss_number] = 1;
            prev_failure = 1;
            sleep(1);
            continue;
        }
        else
        {
            failure[ss_number] = 0;
            if (prev_failure == 1)
            {
                for (int i = 0; i < initial_data[ss_number].number_of_paths; i++)
                {
                    initial_data[ss_number].paths[i].backup_pending[0] = 1;
                    initial_data[ss_number].paths[i].backup_pending[1] = 1;
                }
                backup_pending[ss_number] = 1;
                prev_failure = 0;
                printf(GREEN "Storage server with ip %s and port %d is alive\n" RESET, initial_data[ss_number].ip, initial_data[ss_number].port_number);
            }
        }
        sleep(1);
    }

    return NULL;
}
void *ss_init_thread(void *)
{
    init_root();
    int port_number = SS_PORT;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = inet_addr(IP);

    int bind_success = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_success == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }
    int listen_success = listen(sock, 10);
    if (listen_success == -1)
    {
        perror("Error in listen() function call: ");
        exit(1);
    }
    while (1)
    {
        // printf("Current number of ss is %d\n", curr_number_of_ss);
        // for (int i = 0; i < curr_number_of_ss; i++)
        // {
        //     printf("Original ss index is %d\n", backup_arr[i].original_ss_index);
        //     printf("Replica1 ss index is %d\n", backup_arr[i].replica1_ss_index);
        //     printf("Replica2 ss index is %d\n", backup_arr[i].replica2_ss_index);
        // }

        pthread_t ss_checkalive;
        int arg1 = curr_number_of_ss;
        pthread_create(&ss_checkalive, NULL, checkalive, (void *)&arg1);

        int client_socket;
        struct sockaddr_in client_address;
        int client_address_length = sizeof(client_address);
        client_socket = accept(sock, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1)
        {
            perror("Error in accept() function call: ");
            exit(1);
        }

        printf(BLUE "[NAMING SERVER] Received a connection from a storage server with ip %s and port %d\n" RESET, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        struct data_of_ss initial_data_of_ss;
        memset(&initial_data_of_ss, 0, sizeof(initial_data_of_ss));
        // recv(client_socket, &initial_data_of_ss, sizeof(initial_data_of_ss), 0);
        // printf("Here1\n");
        if (recv(client_socket, &initial_data_of_ss, sizeof(initial_data_of_ss), 0) == -1)
        {
            perror("Error in recv() function call: ");
            exit(1);
        }
        // get ip address of storage server from struct sockaddr_in client_address
        char *ip_address_of_ss = inet_ntoa(client_address.sin_addr);

        for (int i = 0; i < initial_data_of_ss.number_of_paths; i++)
        {
            insert_into_tree_new(initial_data_of_ss.paths[i].path, initial_data_of_ss.paths[i].permissions, ip_address_of_ss, initial_data_of_ss.port_number, initial_data_of_ss.client_port, initial_data_of_ss.s2s_port, initial_data_of_ss.paths[i].dir_or_file);
        }
        initial_data[curr_number_of_ss] = initial_data_of_ss;
        backup_arr[curr_number_of_ss].original_ss_index = curr_number_of_ss;
        strcpy(backup_arr[curr_number_of_ss].original_ss_ip, ip_address_of_ss);
        strcpy(initial_data[curr_number_of_ss].ip, ip_address_of_ss);
        backup_arr[curr_number_of_ss].original_ss_port = initial_data_of_ss.port_number;
        if (curr_number_of_ss == 0)
        {
            // pthread_t ss_backup_update_thread_id;
            int arg1 = curr_number_of_ss;
            // backup_pending[curr_number_of_ss] = 1;
            // pthread_create(&ss_backup_update_thread_id, NULL, ss_backup_update, (void *)(&arg1));
            curr_number_of_ss++;
            continue;
        }
        else if (curr_number_of_ss == 1)
        {
            backup_arr[0].replica1_ss_index = curr_number_of_ss;
            strcpy(backup_arr[0].replica1_ss_ip, ip_address_of_ss);
            backup_arr[0].replica1_ss_port = initial_data_of_ss.port_number;
            backup_arr[curr_number_of_ss].replica1_ss_index = 0;
            strcpy(backup_arr[curr_number_of_ss].replica1_ss_ip, backup_arr[0].original_ss_ip);
            backup_arr[curr_number_of_ss].replica1_ss_port = backup_arr[0].original_ss_port;
            backup_arr[curr_number_of_ss].number_of_references = 1;
            backup_arr[0].number_of_references++;
            // pthread_t ss_backup_update_thread_id;
            // int arg1 = curr_number_of_ss;
            // pthread_create(&ss_backup_update_thread_id, NULL, ss_backup_update, (void *)(&arg1));
            // curr_number_of_ss++;
        }
        else if (curr_number_of_ss == 2)
        {
            backup_arr[0].replica2_ss_index = curr_number_of_ss;
            strcpy(backup_arr[0].replica2_ss_ip, ip_address_of_ss);
            backup_arr[0].replica2_ss_port = initial_data_of_ss.port_number;
            backup_arr[curr_number_of_ss].replica2_ss_index = 0;
            strcpy(backup_arr[curr_number_of_ss].replica2_ss_ip, backup_arr[0].original_ss_ip);
            backup_arr[curr_number_of_ss].replica2_ss_port = backup_arr[0].original_ss_port;
            backup_arr[curr_number_of_ss].number_of_references++;
            backup_arr[0].number_of_references++;
            backup_arr[1].replica2_ss_index = curr_number_of_ss;
            strcpy(backup_arr[1].replica2_ss_ip, ip_address_of_ss);
            backup_arr[1].replica2_ss_port = initial_data_of_ss.port_number;
            backup_arr[curr_number_of_ss].replica1_ss_index = 1;
            strcpy(backup_arr[curr_number_of_ss].replica1_ss_ip, backup_arr[1].original_ss_ip);
            backup_arr[curr_number_of_ss].replica1_ss_port = backup_arr[1].original_ss_port;
            backup_arr[curr_number_of_ss].number_of_references++;
            backup_arr[1].number_of_references++;
            // pthread_t ss_backup_update_thread_id;
            // int arg1 = curr_number_of_ss;
            // pthread_create(&ss_backup_update_thread_id, NULL, ss_backup_update, (void *)(&arg1));
            // curr_number_of_ss++;
        }
        else
        {
            int min_index = 0;
            int min = backup_arr[0].number_of_references;
            for (int i = 1; i < curr_number_of_ss; i++)
            {
                if (backup_arr[i].number_of_references < min)
                {
                    min = backup_arr[i].number_of_references;
                    min_index = i;
                }
            }
            backup_arr[curr_number_of_ss].replica1_ss_index = min_index;
            strcpy(backup_arr[curr_number_of_ss].replica1_ss_ip, backup_arr[min_index].original_ss_ip);
            backup_arr[curr_number_of_ss].replica1_ss_port = backup_arr[min_index].original_ss_port;
            backup_arr[min_index].number_of_references++;
            int min2_index = 0;
            int min2 = backup_arr[0].number_of_references;
            for (int i = 1; i < curr_number_of_ss; i++)
            {
                if (backup_arr[i].number_of_references < min2)
                {
                    if (i == min_index)
                    {
                        continue;
                    }
                    min2 = backup_arr[i].number_of_references;
                    min2_index = i;
                }
            }
            backup_arr[curr_number_of_ss].replica2_ss_index = min2_index;
            strcpy(backup_arr[curr_number_of_ss].replica2_ss_ip, backup_arr[min2_index].original_ss_ip);
            backup_arr[curr_number_of_ss].replica2_ss_port = backup_arr[min2_index].original_ss_port;
            backup_arr[min2_index].number_of_references++;
        }
        backup_pending[curr_number_of_ss] = 1;

        curr_number_of_ss++;
        if (curr_number_of_ss == 2)
        {
            backup_pending[0] = 1;
            backup_pending[1] = 1;
        }
    }
}

void *client_req_handler(void *arg)
{
    pthread_detach(pthread_self());
    int client_socket = *((int *)arg);
    char *msg = (char *)malloc(sizeof(char) * 100);
    memset(msg, 0, 100);
    if (recv(client_socket, msg, 100, 0) == -1)
    {
        perror("Error in recv() function call: ");
        exit(1);
    }
    msg[strlen(msg)] = '\0';
    printf(CYAN "[CLIENT] Received a request from a client : %s\n" RESET, msg);
    char *token = strtok(msg, " ");

    if (strcmp(token, "makedir") == 0)
    {
        char *name_of_dir = strtok(NULL, " ");
        char *path = strtok(NULL, " ");
        int res = makedirnm(name_of_dir, path, client_socket);
        if (res == SUCCESS)
        {
            char *parent_path = (char *)malloc(sizeof(char) * 100);
            strcpy(parent_path, path);
            parent_path[strlen(parent_path)] = '\0';
            int ss_number_to_search;
            for (int ss_number = 0; ss_number < curr_number_of_ss; ss_number++)
            {
                if (strcmp(initial_data[ss_number].paths[0].path, parent_path) == 0)
                {
                    ss_number_to_search = ss_number;
                    break;
                }
            }
            char new_path[100];
            strcpy(new_path, parent_path);
            strcat(new_path, "/");
            strcat(new_path, name_of_dir);
            new_path[strlen(new_path)] = '\0';
            int curr_index_to_add = initial_data[ss_number_to_search].number_of_paths;
            strcpy(initial_data[ss_number_to_search].paths[curr_index_to_add].path, new_path);
            initial_data[ss_number_to_search].paths[curr_index_to_add].dir_or_file = 1;
            initial_data[ss_number_to_search].paths[curr_index_to_add].permissions = 1;
            initial_data[ss_number_to_search].number_of_paths++;
            if (failure[ss_number_to_search] == 0)
            {
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[0] = 1;
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[1] = 1;
                backup_pending[ss_number_to_search] = 1;
            }
            else
            {
                if (failure[backup_arr[ss_number_to_search].replica1_ss_index] == 0)
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 1;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 0;
                }
                else
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 0;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 1;
                }
                recovery_pending[ss_number_to_search] = 1;
            }
        }
    }
    else if (strcmp(token, "deletedir") == 0)
    {
        char *path = strtok(NULL, " ");
        deletedirnm(path, client_socket);
    }
    else if (strcmp(token, "makefile") == 0)
    {
        char *filename = strtok(NULL, " ");
        char *path = strtok(NULL, " ");
        int res = makefilenm(filename, path, client_socket);
        if (res == SUCCESS)
        {
            char *parent_path = (char *)malloc(sizeof(char) * 100);
            strcpy(parent_path, path);
            parent_path[strlen(parent_path)] = '\0';
            int ss_number_to_search;
            for (int ss_number = 0; ss_number < curr_number_of_ss; ss_number++)
            {
                if (strcmp(initial_data[ss_number].paths[0].path, parent_path) == 0)
                {
                    ss_number_to_search = ss_number;
                    break;
                }
            }
            char new_path[100];
            strcpy(new_path, parent_path);
            strcat(new_path, "/");
            strcat(new_path, filename);
            new_path[strlen(new_path)] = '\0';
            int curr_index_to_add = initial_data[ss_number_to_search].number_of_paths;
            strcpy(initial_data[ss_number_to_search].paths[curr_index_to_add].path, new_path);
            initial_data[ss_number_to_search].paths[curr_index_to_add].dir_or_file = 0;
            initial_data[ss_number_to_search].paths[curr_index_to_add].permissions = 1;
            initial_data[ss_number_to_search].number_of_paths++;
            if (failure[ss_number_to_search] == 0)
            {
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[0] = 1;
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[1] = 1;
                backup_pending[ss_number_to_search] = 1;
            }
            else
            {
                if (failure[backup_arr[ss_number_to_search].replica1_ss_index] == 0)
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 1;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 0;
                }
                else
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 0;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 1;
                }
                recovery_pending[ss_number_to_search] = 1;
            }
        }
    }
    else if (strcmp(token, "deletefile") == 0)
    {
        char *path = strtok(NULL, " ");
        deletefilenm(path, client_socket);
    }
    else if (strcmp(token, "copyfile") == 0)
    {
        char *src = strtok(NULL, " ");
        char *dest = strtok(NULL, " ");
        int res = copyfilenm(src, dest, client_socket);
        if (res == SUCCESS)
        {
            int ss_number_to_search;
            for (int ss_number = 0; ss_number < curr_number_of_ss; ss_number++)
            {
                if (strcmp(initial_data[ss_number].paths[0].path, dest) == 0)
                {
                    ss_number_to_search = ss_number;
                    break;
                }
            }
            int curr_index_to_add = initial_data[ss_number_to_search].number_of_paths;
            char *file_name = strrchr(src, '/');
            char *new_path = (char *)malloc(sizeof(char) * 100);
            strcpy(new_path, dest);
            // strcat(new_path,"/");
            strcat(new_path, file_name);
            new_path[strlen(new_path)] = '\0';
            strcpy(initial_data[ss_number_to_search].paths[curr_index_to_add].path, new_path);
            initial_data[ss_number_to_search].paths[curr_index_to_add].dir_or_file = 0;
            initial_data[ss_number_to_search].paths[curr_index_to_add].permissions = 1;
            initial_data[ss_number_to_search].number_of_paths++;
            if (failure[ss_number_to_search] == 0)
            {
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[0] = 1;
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[1] = 1;
                backup_pending[ss_number_to_search] = 1;
            }
            else
            {
                if (failure[backup_arr[ss_number_to_search].replica1_ss_index] == 0)
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 1;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 0;
                }
                else
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 0;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 1;
                }
                recovery_pending[ss_number_to_search] = 1;
            }
        }
    }
    else if (strcmp(token, "copydir") == 0)
    {
        char *src = strtok(NULL, " ");
        char *dest = strtok(NULL, " ");
        int res = copydirnm(src, dest, client_socket);
        if (res == SUCCESS)
        {
            int ss_number_to_search;
            for (int ss_number = 0; ss_number < curr_number_of_ss; ss_number++)
            {
                if (strcmp(initial_data[ss_number].paths[0].path, dest) == 0)
                {
                    ss_number_to_search = ss_number;
                    break;
                }
            }
            int curr_index_to_add = initial_data[ss_number_to_search].number_of_paths;
            strcpy(initial_data[ss_number_to_search].paths[curr_index_to_add].path, src);
            initial_data[ss_number_to_search].paths[curr_index_to_add].dir_or_file = 1;
            initial_data[ss_number_to_search].paths[curr_index_to_add].permissions = 1;
            initial_data[ss_number_to_search].number_of_paths++;
            if (failure[ss_number_to_search] == 0)
            {
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[0] = 1;
                initial_data[ss_number_to_search].paths[curr_index_to_add].backup_pending[1] = 1;
                backup_pending[ss_number_to_search] = 1;
            }
            else
            {
                if (failure[backup_arr[ss_number_to_search].replica1_ss_index] == 0)
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 1;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 0;
                }
                else
                {
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[0] = 0;
                    initial_data[ss_number_to_search].paths[curr_index_to_add].recovery_pending[1] = 1;
                }
                recovery_pending[ss_number_to_search] = 1;
            }
        }
    }
    else if (strcmp(token, "read") == 0)
    {
        char *filename = strtok(NULL, " ");
        readnm(filename, client_socket);
    }
    else if (strcmp(token, "write") == 0)
    {
        /* code */
        char *filename = strtok(NULL, " ");
        int res = writenm(filename, client_socket);
        if (res == SUCCESS)
        {
            for (int ss_number = 0; ss_number < curr_number_of_ss; ss_number++)
            {
                for (int path_index = 0; path_index < initial_data[ss_number].number_of_paths; path_index++)
                {
                    if (strcmp(initial_data[ss_number].paths[path_index].path, filename) == 0)
                    {
                        if (failure[ss_number] == 1)
                        {
                            recovery_pending[ss_number] = 1;
                            if (failure[backup_arr[ss_number].replica1_ss_index] == 1)
                            {
                                initial_data[ss_number].paths[path_index].recovery_pending[1] = 1;
                                initial_data[ss_number].paths[path_index].recovery_pending[0] = 0;
                            }
                            else
                            {
                                initial_data[ss_number].paths[path_index].recovery_pending[0] = 1;
                                initial_data[ss_number].paths[path_index].recovery_pending[1] = 0;
                            }
                            break;
                        }
                        else
                        {
                            backup_pending[ss_number] = 1;
                            initial_data[ss_number].paths[path_index].backup_pending[0] = 1;
                            initial_data[ss_number].paths[path_index].backup_pending[1] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    else if (strcmp(token, "list") == 0)
    {
        int total_number_of_paths = 0;
        int n_ss = curr_number_of_ss;
        for (int i = 0; i < n_ss; i++)
        {
            total_number_of_paths += initial_data[i].number_of_paths;
        }
        char *paths_arr[total_number_of_paths];
        int index = 0;
        for (int i = 0; i < n_ss; i++)
        {
            for (int j = 0; j < initial_data[i].number_of_paths; j++)
            {
                if (index == total_number_of_paths)
                {
                    break;
                }
                paths_arr[index] = (char *)malloc(sizeof(char) * 100);
                strcpy(paths_arr[index], initial_data[i].paths[j].path);
                paths_arr[index][strlen(paths_arr[index])] = '\0';
                printf("Path is %s\n", paths_arr[index]);
                index++;
            }
        }
        listnm(client_socket, total_number_of_paths, paths_arr);
    }

    close(client_socket);
    return NULL;
}

void *client_thread(void *)
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
    server_address.sin_port = htons(CLIENT_PORT);
    server_address.sin_addr.s_addr = inet_addr(IP);

    int bind_success = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_success == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }

    int listen_success = listen(sock, 15);
    if (listen_success == -1)
    {
        perror("Error in listen() function call: ");
        exit(1);
    }

    while (1)
    {
        int client_socket;
        struct sockaddr_in client_address;
        int client_address_length = sizeof(client_address);
        client_socket = accept(sock, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1)
        {
            perror("Error in accept() function call: ");
            exit(1);
        }
        pthread_t client_req_handler_thread_id;
        pthread_create(&client_req_handler_thread_id, NULL, client_req_handler, (void *)&client_socket);
    }
}

int main()
{
    sem_init(&sem[0], 0, 1);
    for (int i = 1; i < 1000; i++)
    {
        sem_init(&sem[i], 0, 0);
    }
    pthread_mutex_init(&init_lock, NULL);
    alive_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (alive_socket == -1)
    {
        perror("Error in socket() function call: ");
        exit(1);
    }

    struct sockaddr_in alive_server_address;
    memset(&alive_server_address, 0, sizeof(alive_server_address));
    alive_server_address.sin_family = AF_INET;
    alive_server_address.sin_port = htons(CHECK_ALIVE_PORT);
    alive_server_address.sin_addr.s_addr = inet_addr(IP);

    int alive_bind_success = bind(alive_socket, (struct sockaddr *)&alive_server_address, sizeof(alive_server_address));
    if (alive_bind_success == -1)
    {
        perror("Error in bind() function call: ");
        exit(1);
    }

    int alive_listen_success = listen(alive_socket, 20);
    if (alive_listen_success == -1)
    {
        perror("Error in listen() function call: ");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
    curr_number_of_ss = 0;
    memset(failure, 0, sizeof(failure));
    // memset(backup_arr, 0, sizeof(backup_arr));
    for (int i = 0; i < 100; i++)
    {
        backup_pending[i] = 0;
        backup_arr[i].original_ss_index = -1;
        backup_arr[i].replica1_ss_index = -1;
        backup_arr[i].replica2_ss_index = -1;
        backup_arr[i].number_of_references = 0;
    }

    pthread_t ss_init_thread_id;
    pthread_create(&ss_init_thread_id, NULL, ss_init_thread, NULL);
    pthread_t client_thread_id;
    pthread_create(&client_thread_id, NULL, client_thread, NULL);
    pthread_t ss_backup_update_thread_id;
    pthread_create(&ss_backup_update_thread_id, NULL, ss_backup_update, NULL);
    pthread_join(client_thread_id, NULL);
    pthread_join(ss_init_thread_id, NULL);
    return 0;
}