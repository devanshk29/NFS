#include "write.h"
extern struct my_struct *curr_files;
void writess(char *path, char *data, int client_soket)
{
    char *cpy_path = (char *)malloc(sizeof(char) * (strlen(path)+10));
    strcpy(cpy_path, ".");
    strcat(cpy_path, path);
    cpy_path[strlen(cpy_path)] = '\0';
    memset(path, 0, sizeof(path));
    strcpy(path, cpy_path);
    path[strlen(path)] = '\0';
    if (access(path, F_OK) != 0)
    {
        int to_send = FILE_NOT_FOUND;
        if (send(client_soket, &to_send, sizeof(int), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        return;
    }
    else if (access(path, W_OK) != 0)
    {
        int to_send = PERMISSION_DENIED;
        if (send(client_soket, &to_send, sizeof(int), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        return;
    }
    
    else
    {
        struct my_struct *s1;
        for(s1=curr_files;s1!=NULL;s1=s1->hh.next)
        {
            printf("%s\n",s1->name);
        }
        char *path2 = (char *)malloc(sizeof(char) * (400));
        for (int i = 1; i < strlen(path); i++)
        {
            path2[i - 1] = path[i];
        }
        path2[strlen(path2)] = '\0';
        
        struct my_struct *s;
        HASH_FIND_STR(curr_files, path2, s);
        if (s==NULL)
        {
            int to_send = FILE_NOT_FOUND;
            printf("Nahi mila2\n");
            if (send(client_soket, &to_send, sizeof(int), 0) < 0)
            {
                perror("Error in send() function call: ");
                return;
            }
            return;
        }
        
        pthread_mutex_lock(&(s->mutex));
        if (s->being_written == 1)
        {
            int to_send = -2;
            if (send(client_soket, &to_send, sizeof(int), 0) < 0)
            {
                perror("Error in send() function call: ");
                pthread_mutex_unlock(&(s->mutex));
                return;
            }
            pthread_mutex_unlock(&(s->mutex));
            return;
        }
        s->being_written = 1;
        pthread_mutex_unlock(&(s->mutex));
        int fd = open(path, O_WRONLY | O_TRUNC);
        if (fd == -1)
        {
            perror("Error in open() function call: ");
            int to_return = 125;
            if (send(client_soket, &to_return, sizeof(int), 0) < 0)
            {
                perror("Error in send() function call: ");
                return;
            }
            return;
        }
        int number_of_bytes_written = write(fd, data, strlen(data));
        if (number_of_bytes_written == -1)
        {
            perror("Error in write() function call: ");
            int to_return = 125;
            if (send(client_soket, &to_return, sizeof(int), 0) < 0)
            {
                perror("Error in send() function call: ");
                return;
            }
            return;
        }
        int to_return = SUCCESS;
        if (send(client_soket, &to_return, sizeof(int), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        pthread_mutex_lock(&(s->mutex));
        s->being_written = 0;
        pthread_mutex_unlock(&(s->mutex));
        return;
    }
    
}