#include "info.h"

void info(char *input)
{
    printf("Input: %s\n",input);
    char *msg=(char*)malloc(sizeof(char)*500);
    strcpy(msg,"info ");
    strcat(msg,input);
    int len=strlen(msg);
    printf("%s\n",msg);
    msg[len]='\0';
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(NM_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    int ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0)
    {
        printf("Error in connecting to naming server\n");
        return;
    }
    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        perror("Error in send() function call: ");
        return;
    }
    int status;
    if (recv(sockfd, &status, sizeof(status), 0) < 0)
    {
        perror("Error in recv() function call: ");
        return;
    }
    if (status == SUCCESS)
    {
        ss_info *ss = (ss_info *)malloc(sizeof(ss_info));
        if (recv(sockfd, ss, sizeof(ss_info), 0) < 0)
        {
            perror("Error in recv() function call: ");
            return;
        }
        int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr2;
        memset(&serv_addr2, 0, sizeof(serv_addr2));
        serv_addr2.sin_family = AF_INET;
        serv_addr2.sin_port=htons(ss->ss_port);
        serv_addr2.sin_addr.s_addr = inet_addr(ss->ss_ip);
        int ret2 = connect(sockfd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2));
        if (ret2 < 0)
        {
            printf("Error in connecting to storage server\n");
            return;
        }
        printf("Connected to storage server\n");
        char *msg2=(char*)malloc(sizeof(char)*500);
        strcpy(msg2,"info ");
        strcat(msg2,input);
        int len2=strlen(msg2);
        msg2[len2]='\0';
        printf("Msg2: %s\n",msg2);
        if (send(sockfd2, msg2, strlen(msg2), 0) < 0)
        {
            perror("Error in send() function call: ");
            return;
        }
        int stat2;
        if (recv(sockfd2, &stat2, sizeof(stat2), 0) < 0)
        {
            perror("Error in recv() function call: ");
            return;
        }
        // if(stat2!=SUCCESS)
        // {
        //     printf("Error in info\n");
        //     return;
        // }
        struct stat file_stat;
        if (recv(sockfd2, &file_stat, sizeof(file_stat), 0) < 0)
        {
            perror("Error in recv() function call: ");
            return;
        }
        printf("File size: %ld\n",file_stat.st_size);
        printf("File permissions: %d\n",file_stat.st_mode);
        printf("File last accessed: %ld\n",file_stat.st_atime);
        printf("File last modified: %ld\n",file_stat.st_mtime);
        printf("File last status change: %ld\n",file_stat.st_ctime);
        close(sockfd2);
    }
    else
    {
        printf("Error in info\n");
    }
}