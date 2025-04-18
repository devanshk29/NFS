#include "makefile.h"
#include "../storage_server.h"


extern int failure[100000];
extern struct replica_info backup_arr[100];
extern struct data_of_ss initial_data[100];

extern lru_entry *head;


int makefilenm(char *filename, char* path , int client_soket)
{
    ss_info ans;

    ss_info cache_ans = find_in_cache(path);

    if(cache_ans.ss_port != -5)
    {
        ans = cache_ans;
    }
    else
    {
        ans = search_path_in_trie(path);
        add_to_cache(path, ans);
    }

    if(ans.ss_port==-1)
    {
        int status = INVALID_PATH;
        printf(YELLOW "Directory not found in makefile\n" RESET);
        if (send(client_soket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    if(ans.dir_or_file==IS_FILE)
    {
        int status = INVALID_PATH;
        printf(YELLOW "Path is a file in makefile\n" RESET);
        if (send(client_soket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    int curr_ss_num=-1;
    for(int i=0;i<100;i++)
    {
        if(ans.ss_port == backup_arr[i].original_ss_port && strcmp(ans.ss_ip,backup_arr[i].original_ss_ip)==0)
        {
            curr_ss_num=i;
            break;
        }
    }

    if(curr_ss_num==-1)
    {
        int status = INVALID_PATH;
        printf(YELLOW "Directory not found in makefile\n" RESET);
        if (send(client_soket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }

    ss_info copy=ans;

    int temp;
    if(failure[curr_ss_num]==1)
    {
        if(failure[backup_arr[curr_ss_num].replica1_ss_index]==1)
        {
            if(failure[backup_arr[curr_ss_num].replica2_ss_index]==1)
            {
                int status = SS_DOWN;
                printf(YELLOW "Storage server is down in makefile\n" RESET);
                if (send(client_soket, &status, sizeof(status), 0) == -1)
                {
                    perror("Error in send() function call: ");
                    return -1;
                }
                return -1;
            }
            else
            {
                temp=backup_arr[curr_ss_num].replica2_ss_index;
            }
        }
        else
        {
            temp=backup_arr[curr_ss_num].replica1_ss_index;
        }

        ans.ss_port=initial_data[temp].port_number;
        ans.s2s_port=initial_data[temp].s2s_port;
        ans.client_port=initial_data[temp].client_port;
        strcpy(ans.ss_ip,initial_data[temp].ip);
    }
    else
    {
        temp=curr_ss_num;
    }

    int sock_ss = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_ss == -1)
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
    if(connect_success == -1)
    {
        perror("Error in connect() function call: ");
        return -1;
    }

    char *msg_to_ss = (char *)malloc(sizeof(char) * 100);
    strcpy(msg_to_ss, "makefile ");
    strcat(msg_to_ss, filename);
    strcat(msg_to_ss, " ");
    strcat(msg_to_ss, path);

    if(send(sock_ss, msg_to_ss, strlen(msg_to_ss), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    int status;
    if(recv(sock_ss, &status, sizeof(status), 0) == -1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }

    close(sock_ss);

    char *new_path = (char *)malloc(sizeof(char) * (strlen(path) + strlen(filename) + 2));
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, filename);
    new_path[strlen(new_path)] = '\0';

    if(status == SUCCESS)
    {
        insert_into_tree_new(new_path, 1, copy.ss_ip, copy.ss_port, copy.client_port, copy.s2s_port, 0);
        printf(GREEN "File created successfully\n" RESET);
    }
    else
    {
        printf(RED "Error in creating file\n" RESET);
    }

    if(send(client_soket, &status, sizeof(status), 0) == -1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    

    return status;
}