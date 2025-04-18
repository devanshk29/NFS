#include "list.h"

// extern int curr_number_of_ss;
struct details_of_path
{
    char path[400];
    int permissions;
    int dir_or_file;
    int backup_pending[2];
    int recovery_pending[2];
};

struct data_of_ss
{
    int port_number;
    int number_of_paths;
    int s2s_port;
    int client_port;
    struct details_of_path paths[512];
    char ip[20];
};

// extern struct data_of_ss initial_data[100];
void listnm(int client_sockfd,int number_to_send,char * paths_arr[])
{
    // printf("Sending number: %d\n",number_to_send);
    int to_send_number_of_ss=number_to_send;
    if (send(client_sockfd, &to_send_number_of_ss, sizeof(to_send_number_of_ss), 0) < 0)
    {
        perror("Error in send() function call: ");
        return;
    }
    usleep(100);
    for (int i = 0; i < number_to_send; i++)
    {
        char* to_send=(char *)malloc(sizeof(char)*500);
        strcpy(to_send,paths_arr[i]);
        to_send[strlen(to_send)]='\0';
        // printf("Sending %s\n",to_send);
        if (send(client_sockfd, to_send, strlen(to_send), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        usleep(100);
    }

    close(client_sockfd);
    return;
}