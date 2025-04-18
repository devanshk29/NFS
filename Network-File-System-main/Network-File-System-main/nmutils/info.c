#include "info.h"

void infonm(int client_socket, char *path)
{
    ss_info ret=search_path_in_trie(path);
    if (ret.ss_port == -1)
    {
        int ret_val=FILE_NOT_FOUND;
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
        int ret_val=SUCCESS;
        if (send(client_socket, &ret_val, sizeof(ret_val), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        usleep(1000);
        if (send(client_socket, &ret, sizeof(ret), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
    }
    
}