#include "read.h"

void read_file(char *input)
{
    char *cpy_command = (char *)malloc(sizeof(char) * strlen(input));
    strcpy(cpy_command, input);
    cpy_command[strlen(input)] = '\0';

    char *command = strtok(cpy_command, " ");

    char *filename = strtok(NULL, " ");
    if (filename == NULL)
    {
        printf("Usage: read <filename(with path)> \n");
        return;
    }

    char *token = strtok(NULL, " ");
    if (token != NULL)
    {
        printf("Usage: read <filename(with path)> \n");
        return;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        printf("Error in creating socket\n");
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NM_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    int ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (ret < 0)
    {
        printf("Error in connecting to naming server\n");
        return;
    }

    char *msg = (char *)malloc(sizeof(char) * 100);
    strcpy(msg, "read ");
    strcat(msg, filename);

    int len = strlen(msg);
    msg[len] = '\0';

    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }

    int status;

    if (recv(sockfd, &status, sizeof(int), 0) < 0)
    {
        printf("Error in receiving data from naming server\n");
        return;
    }

    if (status == SUCCESS)
    {
        ss_info *ss = (ss_info *)malloc(sizeof(ss_info));
        if (recv(sockfd, ss, sizeof(ss_info), 0) < 0)
        {
            printf("Error in receiving data from naming server\n");
            return;
        }

        int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd2 < 0)
        {
            printf("Error in creating socket\n");
            return;
        }

        struct sockaddr_in serv_addr2;
        memset(&serv_addr2, 0, sizeof(serv_addr2));
        serv_addr2.sin_family = AF_INET;
        serv_addr2.sin_port = htons(ss->client_port);
        serv_addr2.sin_addr.s_addr = inet_addr(ss->ss_ip);
        usleep(10);
        int ret2 = connect(sockfd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2));
        if (ret2 < 0)
        {
            printf("Error in connecting to storage server\n");
            return;
        }

        char *msg2 = (char *)malloc(sizeof(char) * 100);
        strcpy(msg2, "read ");
        strcat(msg2, filename);

        int len2 = strlen(msg2);
        msg2[len2] = '\0';

        if (send(sockfd2, msg2, strlen(msg2), 0) < 0)
        {
            printf("Error in sending data to storage server\n");
            return;
        }
        int number_of_bytes_to_read;
        if (recv(sockfd2, &number_of_bytes_to_read, sizeof(int), 0) < 0)
        {
            printf("Error in receiving data from storage server\n");
            return;
        }
        if (number_of_bytes_to_read == -2)
        {
            printf("Read permission denied\n");
        }
        else
        {
            int number_of_bytes_recieved = 0;
            while (number_of_bytes_recieved < number_of_bytes_to_read)
            {
                int left = number_of_bytes_to_read - number_of_bytes_recieved;
                if (left > 100)
                {
                    left = 100;
                }
                char *buffer = (char *)malloc(sizeof(char) * left);
                memset(buffer, 0, sizeof(buffer));
                int number_of_bytes_recieved_now = recv(sockfd2, buffer, sizeof(buffer), 0);
                if (number_of_bytes_recieved_now == 0)
                {
                    break;
                }
                number_of_bytes_recieved += number_of_bytes_recieved_now;
                printf("%s", buffer);
            }
            printf("\n");
        }
    }
    else if (status == FILE_NOT_FOUND)
    {
        printf("File not found\n");
    }
    else
    {
        printf("Unknown error during read\n");
    }
}