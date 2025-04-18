#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("Socket creation failed\n");
        exit(0);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54678);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int connect_status = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connect_status < 0)
    {
        printf("Connection failed\n");
        exit(0);
    }
    char file_name[100];
    read(server_fd, file_name, sizeof(file_name));
    printf("File name recieved is %s\n", file_name);
    // int file_fd = open(file_name, O_RDONLY);
    // create
    int file_fd = open(file_name, O_RDONLY | O_WRONLY | O_CREAT, 0666);
    int number_of_bytes_to_recieve;
    read(server_fd, &number_of_bytes_to_recieve, sizeof(number_of_bytes_to_recieve));
    printf("Number of bytes to recieve is %d\n", number_of_bytes_to_recieve);
    char buffer[100];
    int number_of_bytes_recieved = 0;
    while (1)
    {
        if (number_of_bytes_recieved >= number_of_bytes_to_recieve)
        {
            break;
        }
        int number_of_bytes_recieved_now = read(server_fd, buffer, sizeof(buffer));
        if (number_of_bytes_recieved_now == 0)
        {
            /* code */
            break;
        }

        write(file_fd, buffer, number_of_bytes_recieved_now);
        number_of_bytes_recieved += number_of_bytes_recieved_now;
    }

    printf("File recieved\n");
    return 0;
}