#include "read.h"
#include "../storage_server.h"

extern int failure[100000];
extern struct replica_info backup_arr[100];
extern struct data_of_ss initial_data[100];

extern lru_entry *head;

void readnm(char *path, int client_soket)
{

    ss_info cache_ans = find_in_cache(path);
    ss_info ans;

    if (cache_ans.ss_port != -5)
    {
        ans = cache_ans;
    }
    else
    {
        ans = search_path_in_trie(path);
        add_to_cache(path, ans);
    }

    if (ans.ss_port == -1)
    {
        int status = FILE_NOT_FOUND;
        printf(YELLOW "File not found in read\n" RESET);
        if (send(client_soket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return;
        }
        return;
    }
    else
    {
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
            int status = FILE_NOT_FOUND;
            printf(YELLOW "File not found in read\n" RESET);
            if (send(client_soket, &status, sizeof(status), 0) == -1)
            {
                perror("Error in send() function call: ");
                return;
            }
            return;
        }
        int temp;
        if (failure[curr_ss_num] == 1)
        {
            if (failure[backup_arr[curr_ss_num].replica1_ss_index] == 1)
            {
                if (failure[backup_arr[curr_ss_num].replica2_ss_index] == 1)
                {
                    int status = SS_DOWN;
                    printf(YELLOW "All 3 storage servers are down in read\n" RESET);
                    if (send(client_soket, &status, sizeof(status), 0) == -1)
                    {
                        perror("Error in send() function call: ");
                        return;
                    }
                    return;
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

        int status = SUCCESS;
        printf(GREEN "File found in read\n" RESET);
        if (send(client_soket, &status, sizeof(status), 0) == -1)
        {
            perror("Error in send() function call: ");
            return;
        }
        if (send(client_soket, &ans, sizeof(ans), 0) == -1)
        {
            perror("Error in send() function call: ");
            return;
        }
    }
}