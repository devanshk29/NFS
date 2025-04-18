#include "info.h"

// void info_ss1(int client_socket, char *path)
// {
//     struct stat file_stat;
//     memset(&file_stat, 0, sizeof(file_stat));
//     if (stat(path, &file_stat) < 0)
//     {
//         perror("Error in stat() function call: ");
//         int ret_val=FILE_NOT_FOUND;
//         if (send(client_socket, &ret_val, sizeof(ret_val), 0) < 0)
//         {
//             perror("Error in send() function call: ");
//             return;
//         }
//         close(client_socket);
//         return;
//     }
//     else
//     {
//         int ret_val=SUCCESS;
//         if (send(client_socket, &ret_val, sizeof(ret_val), 0) < 0)
//         {
//             perror("Error in send() function call: ");
//             return;
//         }
//         usleep(1000);
//         if (send(client_socket, &file_stat, sizeof(file_stat), 0) < 0)
//         {
//             perror("Error in send() function call: ");
//             return;
//         }
//     }
// }