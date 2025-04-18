#include "write.h"
#include "../storage_server.h"

extern int failure[100000];
extern struct replica_info backup_arr[100];
extern struct data_of_ss initial_data[100];

extern lru_entry *head;


int writenm(char *path, int client_soket)
{
    ss_info res;

    ss_info cache_ans = find_in_cache(path);

    if (cache_ans.ss_port != -5)
    {
        res = cache_ans;
    }
    else
    {
        res = search_path_in_trie(path);
        add_to_cache(path, res);
    }

    if (res.ss_port == -1)
    {
        int to_send = FILE_NOT_FOUND;
        printf(YELLOW "File not found in write\n" RESET);
        if (send(client_soket, &to_send, sizeof(int), 0) < 0)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        return -1;
    }
    else
    {
        int curr_ss_num = -1;
        for (int i = 0; i < 100; i++)
        {
            if (res.ss_port == backup_arr[i].original_ss_port && strcmp(res.ss_ip, backup_arr[i].original_ss_ip) == 0)
            {
                curr_ss_num = i;
                break;
            }
        }
        if (curr_ss_num == -1)
        {
            int to_send = FILE_NOT_FOUND;
            printf(YELLOW "File not found in write\n" RESET);
            if (send(client_soket, &to_send, sizeof(int), 0) < 0)
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
                    int to_send = SS_DOWN;
                    if (send(client_soket, &to_send, sizeof(int), 0) < 0)
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
            res.ss_port = initial_data[temp].port_number;
            res.s2s_port = initial_data[temp].s2s_port;
            res.client_port = initial_data[temp].client_port;
            strcpy(res.ss_ip, initial_data[temp].ip);
        }
        else
        {
            temp = curr_ss_num;
        }

        int to_send = SUCCESS;
        printf(GREEN "File found in write\n" RESET);
        if (send(client_soket, &to_send, sizeof(int), 0) < 0)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        usleep(1000);
        if (send(client_soket, &res, sizeof(ss_info), 0) < 0)
        {
            perror("Error in send() function call: ");
            return -1;
        }
        int status;
        if (recv(client_soket, &status, sizeof(int), 0) < 0)
        {
            perror("Error in recv() function call: ");
            return -1;
        }
        if (status == SUCCESS)
        {
            printf(GREEN "File written successfully\n" RESET);
            return 0;
        }
        else
        {
            return -1;
        }
    }
}