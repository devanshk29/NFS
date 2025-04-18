#include "read.h"

void readss(char *path2, int client_soket)
{
    // check for the read premissions
    // use the struct stat
    char *path = (char *)malloc(sizeof(char) * (strlen(path2) + 2));
    strcpy(path,".");
    strcat(path,path2);
    path[strlen(path2) + 1] = '\0' ;
    struct stat file_stat;
    int file_status = stat(path, &file_stat);
    if (file_status < 0)
    {
        printf("File does not exist\n");
        // close(client_soket);
        return;
    }
    if (!(file_stat.st_mode & S_IRUSR))
    {
        printf("File does not have read permission\n");
        int err=-2;
        send(client_soket,&err,sizeof(err),0);
        // close(client_soket);
        return;
    }
    int send_size=file_stat.st_size;
    send(client_soket,&send_size,sizeof(send_size),0);
    int number_of_bytes_sent=0;
    int number_of_bytes_to_send=send_size;
    int file_fd=open(path,O_RDONLY);
    char buffer[100];
    while(1)
    {
        if(number_of_bytes_sent>=number_of_bytes_to_send)
        {
            break;
        }
        int number_of_bytes_to_send_now=read(file_fd,buffer,sizeof(buffer));
        if(number_of_bytes_to_send_now==0)
        {
            break;
        }
        // int sent_bytes=write(client_soket,buffer,number_of_bytes_to_send_now);
        int sent_bytes=send(client_soket,buffer,number_of_bytes_to_send_now,0);
        if(sent_bytes<0)
        {
            printf("Error in sending file\n");
            close(client_soket);
            return;
        }
        number_of_bytes_sent+=sent_bytes;
    }
}