#include "copyfile.h"
#include "../storage_server.h"

extern int failure[100000];
extern struct replica_info backup_arr[100];
extern struct data_of_ss initial_data[100];

int copyfilenm(char *src, char *dest, int client_socket)
{
    ss_info ans;

    ss_info cache_ans = find_in_cache(src);

    if (cache_ans.ss_port != -5)
    {
        ans = cache_ans;
    }
    else
    {
        ans = search_path_in_trie(src);
        add_to_cache(src, ans);
    }


    if (ans.ss_port == -1)
    {
        int status = SRC_NOT_FOUND;
        printf(YELLOW "Source not found in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    if (ans.dir_or_file == IS_DIR)
    {
        int status = SRC_IS_DIR;
        printf(YELLOW "Source is a directory in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }
    ss_info copy1 = ans;

    int curr_ss_num = -1;
    for (int i = 0; i < 100; i++)
    {
        if (ans.ss_port == backup_arr[i].original_ss_port && strcmp(ans.ss_ip, backup_arr[i].original_ss_ip) == 0)
        {
            curr_ss_num = i;
            break;
        }
    }

    if (curr_ss_num == -1)
    {
        int status = SRC_NOT_FOUND;
        printf(YELLOW "Source not found in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    int temp;
    if (failure[curr_ss_num] == 1)
    {
        if (failure[backup_arr[curr_ss_num].replica1_ss_index] == 1)
        {
            if (failure[backup_arr[curr_ss_num].replica2_ss_index] == 1)
            {
                int status = SS_DOWN;
                printf(YELLOW "Source storage server down in copyfilenm\n" RESET);
                if (send(client_socket, &status, sizeof(status), 0) == -1)
                {
                    perror("Error in send() function call: ");
                    return -1;
                }
                return -1;
            }
            else
            {
                temp = backup_arr[curr_ss_num].replica2_ss_index;
            }
        }
        else
        {
            temp = backup_arr[curr_ss_num].replica1_ss_index;
        }

        ans.ss_port = initial_data[temp].port_number;
        ans.s2s_port = initial_data[temp].s2s_port;
        ans.client_port = initial_data[temp].client_port;
        strcpy(ans.ss_ip, initial_data[temp].ip);
    }
    else
    {
        temp = curr_ss_num;
    }

    ss_info ans1;

    ss_info cache_ans1 = find_in_cache(dest);

    if (cache_ans1.ss_port != -5)
    {
        ans1 = cache_ans1;
    }
    else
    {
        ans1 = search_path_in_trie(dest);
        add_to_cache(dest, ans1);
    }


    if (ans1.ss_port == -1)
    {
        int status = DEST_NOT_FOUND;
        printf(YELLOW "Destination not found in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    if (ans1.dir_or_file == IS_FILE)
    {
        int status = DEST_IS_FILE;
        printf(YELLOW "Destination is a file in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }
    ss_info copy2 = ans1;

    int curr_ss_num1 = -1;
    for (int i = 0; i < 100; i++)
    {
        if (ans1.ss_port == backup_arr[i].original_ss_port && strcmp(ans1.ss_ip, backup_arr[i].original_ss_ip) == 0)
        {
            curr_ss_num1 = i;
            break;
        }
    }

    if (curr_ss_num1 == -1)
    {
        int status = DEST_NOT_FOUND;
        printf(YELLOW "Destination not found in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    int temp1;
    if (failure[curr_ss_num1] == 1)
    {
        if (failure[backup_arr[curr_ss_num1].replica1_ss_index] == 1)
        {
            if (failure[backup_arr[curr_ss_num1].replica2_ss_index] == 1)
            {
                int status = SS_DOWN;
                printf(YELLOW "Destination storage server down in copyfilenm\n" RESET);
                if (send(client_socket, &status, sizeof(status), 0) == -1)
                {
                    perror("Error in send() function call: ");
                    return -1;
                }
                return -1;
            }
            else
            {
                temp1 = backup_arr[curr_ss_num1].replica2_ss_index;
            }
        }
        else
        {
            temp1 = backup_arr[curr_ss_num1].replica1_ss_index;
        }

        ans1.ss_port = initial_data[temp1].port_number;
        ans1.s2s_port = initial_data[temp1].s2s_port;
        ans1.client_port = initial_data[temp1].client_port;
        strcpy(ans1.ss_ip, initial_data[temp1].ip);
    }
    else
    {
        temp1 = curr_ss_num1;
    }

    if (ans.s2s_port == ans1.s2s_port && strcmp(ans.ss_ip, ans1.ss_ip) == 0)
    {
        int sock_ss = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ss == -1)
        {
            perror("Error in socket() function call: ");
            return -1;
        }
        struct sockaddr_in server_address_ss;
        memset(&server_address_ss, 0, sizeof(server_address_ss));
        server_address_ss.sin_family = AF_INET;
        server_address_ss.sin_port = htons(ans.ss_port);
        server_address_ss.sin_addr.s_addr = inet_addr(ans.ss_ip);

        int connect_success = connect(sock_ss, (struct sockaddr *)&server_address_ss, sizeof(server_address_ss));
        if (connect_success == -1)
        {
            perror("Error in connect() function call: ");
            return -1;
        }

        char *msg_to_ss = (char *)malloc(sizeof(char) * 100);
        strcpy(msg_to_ss, "copyfile same ");
        strcat(msg_to_ss, src);
        strcat(msg_to_ss, " ");
        strcat(msg_to_ss, dest);

        msg_to_ss[strlen(msg_to_ss)] = '\0';
        if (send(sock_ss, msg_to_ss, strlen(msg_to_ss), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }

        int status;
        if (recv(sock_ss, &status, sizeof(status), 0) == -1)
        {
            perror("Error in recv() function call: ");
            return -1;
        }

        if (status == SUCCESS)
        {
            int status = SUCCESS;
            printf(GREEN "File copied successfully in copyfilenm\n" RESET);
            if (send(client_socket, &status, sizeof(status), 0) == -1)
            {
                perror("Error in send() function call: ");
                return -1;
            }
            char *new_path = (char *)malloc(sizeof(char) * (strlen(dest) + strlen(src) + 2));
            strcpy(new_path, dest);
            char *temp = strrchr(src, '/');
            strcat(new_path, temp);
            new_path[strlen(new_path)] = '\0';
            insert_into_tree_new(new_path, 1, copy1.ss_ip, copy1.ss_port, copy1.client_port, copy1.s2s_port, 0);
            return SUCCESS;
        }
        else
        {
            int status = COPY_ERROR;
            printf(RED "Error in copying file in copyfilenm\n" RESET);
            if (send(client_socket, &status, sizeof(status), 0) == -1)
            {
                perror("Error in send() function call: ");
                return -1;
            }
            return -1;
        }

        return -1;
    }


    int sock_ss = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ss == -1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss;
    memset(&server_address_ss, 0, sizeof(server_address_ss));
    server_address_ss.sin_family = AF_INET;
    server_address_ss.sin_port = htons(ans.ss_port);
    server_address_ss.sin_addr.s_addr = inet_addr(ans.ss_ip);

    int connect_success = connect(sock_ss, (struct sockaddr *)&server_address_ss, sizeof(server_address_ss));
    if (connect_success == -1)
    {
        perror("Error in connect() function call: ");
        return -1;
    }

    char *msg_to_ss = (char *)malloc(sizeof(char) * 100);
    strcpy(msg_to_ss, "copyfile src ");
    strcat(msg_to_ss, src);

    msg_to_ss[strlen(msg_to_ss)] = '\0';
    if (send(sock_ss, msg_to_ss, strlen(msg_to_ss), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    usleep(1000);

    if (send(sock_ss, &ans1, sizeof(ans1), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    int sock_ss1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ss1 == -1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss1;
    memset(&server_address_ss1, 0, sizeof(server_address_ss1));
    server_address_ss1.sin_family = AF_INET;
    server_address_ss1.sin_port = htons(ans1.ss_port);
    server_address_ss1.sin_addr.s_addr = inet_addr(ans1.ss_ip);

    int connect_success1 = connect(sock_ss1, (struct sockaddr *)&server_address_ss1, sizeof(server_address_ss1));
    if (connect_success1 == -1)
    {
        perror("Error in connect() function call: ");
        return -1;
    }

    char *msg_to_ss1 = (char *)malloc(sizeof(char) * 100);
    strcpy(msg_to_ss1, "copyfile dest ");
    strcat(msg_to_ss1, dest);

    msg_to_ss1[strlen(msg_to_ss1)] = '\0';

    if (send(sock_ss1, msg_to_ss1, strlen(msg_to_ss1), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    usleep(1000);

    if (send(sock_ss1, &ans, sizeof(ans), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }


    int status;
    if (recv(sock_ss, &status, sizeof(status), 0) == -1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }

    int status1;
    if (recv(sock_ss1, &status1, sizeof(status1), 0) == -1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }

    if (status == SUCCESS && status1 == SUCCESS)
    {
        int status = SUCCESS;
        printf(GREEN "File copied successfully in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        char *new_path = (char *)malloc(sizeof(char) * (strlen(dest) + strlen(src) + 2));
        strcpy(new_path, dest);
        char *temp = strrchr(src, '/');
        strcat(new_path, temp);
        new_path[strlen(new_path)] = '\0';
        insert_into_tree_new(new_path, 1, copy2.ss_ip, copy2.ss_port, copy2.client_port, copy2.s2s_port, 0);

        return SUCCESS;
    }
    else
    {
        int status = COPY_ERROR;
        printf(RED "Error in copying file in copyfilenm\n" RESET);
        if (send(client_socket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    close(sock_ss);
    close(sock_ss1);
    return -1;
}