#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/stat.h>
#include<fcntl.h>
int main()
{
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0)
    {
        printf("Socket creation failed\n");
        exit(0);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(54678);
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    int bind_status=bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(bind_status<0)
    {
        printf("Binding failed\n");
        exit(0);
    }
    int listen_status=listen(server_fd,10);
    if(listen_status<0)
    {
        printf("Listening failed\n");
        exit(0);
    }
    struct sockaddr_in client_addr;
    int client_addr_len=sizeof(client_addr);
    int client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&client_addr_len);
    if(client_fd<0)
    {
        printf("Accepting failed\n");
        exit(0);
    }
    // int n_to_send=5;
    // write(client_fd,&n_to_send,sizeof(n_to_send));
    // fflush(server_fd);
    // n_to_send=6;
    // write(client_fd,&n_to_send,sizeof(n_to_send));
    // printf("Connection done\n");
    // printf("Now start messaging\n");
    printf("Enter the relative path of hte file to send\n");
    char file_path[100];
    scanf("%s",file_path);
    struct stat file_stat;
   
    int file_status=stat(file_path,&file_stat);
    if(file_status<0)
    {
        printf("File does not exist\n");
        exit(0);
    }
    
    if(!(file_stat.st_mode & S_IRUSR))
    {
        printf("File does not have read permission\n");
        exit(0);
    }
    int send_size=file_stat.st_size;
    
    char file_name[100];
    int i=strlen(file_path)-1;
    while(i>=0 && file_path[i]!='/')
    {
        i--;
    }
    i++;
    int j=0;
    while(i<strlen(file_path))
    {
        file_name[j]=file_path[i];
        i++;
        j++;
    }
    file_name[j]='\0';
    write(client_fd,file_name,sizeof(file_name));
    write(client_fd,&send_size,sizeof(send_size));
    int number_of_bytes_sent=0;
    int number_of_bytes_to_send=send_size;
    int file_fd=open(file_path,O_RDONLY);
    char buffer[100];
    while(number_of_bytes_sent<send_size)
    {
        int number_of_bytes_read=read(file_fd,buffer,sizeof(buffer));
        if (number_of_bytes_read<0)
        {
            printf("Error in reading file\n");
            exit(0);
        }
        if (number_of_bytes_read==0)
        {
            printf("File reading done\n");
            break;
        }
        int sent_bytes=write(client_fd,buffer,number_of_bytes_read);
        if(sent_bytes<0)
        {
            printf("Error in sending file\n");
            exit(0);
        }
        number_of_bytes_sent+=sent_bytes;
    }
    printf("sending done\n");
    return 0;
}