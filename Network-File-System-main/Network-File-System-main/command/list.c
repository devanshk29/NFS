#include "list.h"

void list()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    strcpy(msg, "list");
    int len = strlen(msg);
    msg[len] = '\0';
    if (send(sockfd, msg, strlen(msg), 0) < 0)
    {
        perror("Error in send() function call: ");
        return;
    }
    int number_of_ss;
    if (recv(sockfd, &number_of_ss, sizeof(number_of_ss), 0) < 0)
    {
        perror("Error in recv() function call: ");
        return;
    }
    printf("Number of paths: %d\n", number_of_ss);
    for (int i = 0; i < number_of_ss; i++)
    {
        char data1[500];
        printf("Data of ss %d\n", i);
        memset(&data1, 0, sizeof(data1));
        // usleep(1000);
        if (recv(sockfd, &data1, sizeof(data1), 0) < 0)
        {
            perror("Error in recv() function call: ");
            return;
        }
        printf("%s\n", data1);
    }
    usleep(2000000);
    close(sockfd);
}