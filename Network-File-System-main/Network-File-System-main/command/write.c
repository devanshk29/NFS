#include "write.h"

void write_file(char *input)
{
    char *cpy_command = (char *)malloc(sizeof(char) * strlen(input));
    strcpy(cpy_command, input);
    cpy_command[strlen(input)] = '\0';

    char *command = strtok(cpy_command, " ");

    char *filename = strtok(NULL, " ");
    if (filename == NULL)
    {
        printf("Usage: write <filename(with path)> data\n");
        return;
    }
    // printf("%s\n",cpy_command);
    char *data = strtok(NULL, "\n");
    if (data == NULL)
    {
        printf("Usage: write <filename(with path)> data\n");
        return;
    }
    // printf("%s\n",data);
    // return;
    // char *token = strtok(NULL, "\"");
    // if (token == NULL)
    // {
    //     printf("Usage: write <filename(with path)> data\n");
    //     return;
    // }
    // printf("%s\n",token);
    // return;
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
    strcpy(msg, "write ");
    strcat(msg, filename);

    int len = strlen(msg);
    msg[len] = '\0';

    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        printf("Error in sending data to naming server\n");
        return;
    }

    int status;

    if (recv(sockfd, &status, sizeof(status), 0) < 0)
    {
        printf("Error in receiving data from naming server\n");
        return;
    }
    
    else if (status == SUCCESS)
    {
        ss_info *ss = (ss_info *)malloc(sizeof(ss_info));
        if (recv(sockfd, ss, sizeof(ss_info), 0) < 0)
        {
            printf("Error in receiving data from naming server\n");
            return;
        }

        // close(sockfd);
        // printf("Storage server ip: %s\n", ss->ss_ip);
        // printf("Storage server port: %d\n", ss->ss_port);

        int ss_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (ss_sockfd < 0)
        {
            printf("Error in creating socket\n");
            close(sockfd);
            return;
        }

        struct sockaddr_in ss_serv_addr;
        memset(&ss_serv_addr, 0, sizeof(ss_serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(ss->client_port);
        serv_addr.sin_addr.s_addr = inet_addr(ss->ss_ip);

        // printf("port %d\n", ss->client_port);
        // printf("ip %s\n",ss->ss_ip);

        int ret = connect(ss_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        if (ret < 0)
        {
            printf("Error in connecting to storage server\n");
            close(sockfd);
            return;
        }

        // printf("Yaha tak aa gaye\n");
        char *msg2 = (char *)malloc(sizeof(char) * 100);
        memset(msg2, 0, 100);
        strcpy(msg2, "write ");
        strcat(msg2, filename);
        strcat(msg2, " ");
        strcat(msg2, data);

        msg2[strlen(msg2)] = '\0';
        printf("%s\n",msg2);
        // printf("Yaha tak bhi aa gaye\n");
        if (send(ss_sockfd, msg2, strlen(msg2), 0) < 0)
        {
            printf("Error in sending data to storage server\n");
            close(sockfd);
            return;
        }
        // printf("here15\n");
        int write_status;
        if (recv(ss_sockfd, &write_status, sizeof(write_status), 0) < 0)
        {
            printf("Error in receiving data from storage server\n");
            close(sockfd);
            return;
        }
        // printf("here16\n");
        if (write_status == SUCCESS)
        {
            printf("File written successfully\n");
            int status_to_nm = SUCCESS;
            if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
            {
                printf("Error in sending data to naming server\n");
                close(sockfd);
                return;
            }
            close(sockfd);
            return;
        }
        else if (write_status == FILE_NOT_FOUND)
        {
            printf("File not found\n");
            int status_to_nm = FILE_NOT_FOUND;
            if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
            {
                printf("Error in sending data to naming server\n");
                close(sockfd);
                return;
            }
            close(sockfd);
            return;
        }
        else if (write_status==-2)
        {
            printf("Some other client is writing to the file\n");
            int status_to_nm = -2;
            if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
            {
                printf("Error in sending data to naming server\n");
                close(sockfd);
                return;
            }
            close(sockfd);
        }
        
        else
        {
            printf("Error in writing file\n");
            int status_to_nm = 125;
            if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
            {
                printf("Error in sending data to naming server\n");
                close(sockfd);
                return;
            }
            close(sockfd);
            return;
        }
        // printf("here17\n");
    }
    else if (status == FILE_NOT_FOUND)
    {
        printf("File not found111\n");
        int status_to_nm = FILE_NOT_FOUND;
        if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
        {
            printf("Error in sending data to naming server\n");
            close(sockfd);
            return;
        }

        close(sockfd);
        return;
    }
    else
    {
        printf("Error in writing file\n");
        int status_to_nm = 125;
        if (send(sockfd, &status_to_nm, sizeof(status_to_nm), 0) < 0)
        {
            printf("Error in sending data to naming server\n");
            close(sockfd);
            return;
        }
        close(sockfd);
        return;
    }
}