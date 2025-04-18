#include "backup.h"


int backupdir(ss_info sender_info1,ss_info receiver_info1,char *dir_path)
{
    ss_info sender_info,receiver_info;
    sender_info.ss_port=sender_info1.ss_port;
    strcpy(sender_info.ss_ip,sender_info1.ss_ip);
    sender_info.s2s_port=sender_info1.s2s_port;
    sender_info.dir_or_file=sender_info1.dir_or_file;
    receiver_info.ss_port=receiver_info1.ss_port;
    strcpy(receiver_info.ss_ip,receiver_info1.ss_ip);
    receiver_info.s2s_port=receiver_info1.s2s_port;
    receiver_info.dir_or_file=receiver_info1.dir_or_file;
    sender_info.ss_ip[strlen(sender_info.ss_ip)]='\0';
    receiver_info.ss_ip[strlen(receiver_info.ss_ip)]='\0';
   
    int sock_ss=socket(AF_INET,SOCK_STREAM,0);
    if(sock_ss==-1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss;
    memset(&server_address_ss,0,sizeof(server_address_ss)); 
    server_address_ss.sin_family=AF_INET;
    server_address_ss.sin_port=htons(sender_info.ss_port);
    server_address_ss.sin_addr.s_addr=inet_addr(sender_info.ss_ip);
    int connect_status=connect(sock_ss,(struct sockaddr*)&server_address_ss,sizeof(server_address_ss));
    if(connect_status==-1)
    {
        perror("Error in connect() function call 11: ");
        return -1;
    }
    char *msg_to_ss=(char*)malloc(sizeof(char)*100);
    strcpy(msg_to_ss,"copydir src ");
    strcat(msg_to_ss,dir_path);
    msg_to_ss[strlen(msg_to_ss)]='\0';
    if(send(sock_ss,msg_to_ss,strlen(msg_to_ss),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    usleep(1000);

    if(send(sock_ss,&receiver_info,sizeof(receiver_info),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    int sock_ss1=socket(AF_INET,SOCK_STREAM,0);
    if(sock_ss1==-1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss1;
    memset(&server_address_ss1,0,sizeof(server_address_ss1));
    server_address_ss1.sin_family=AF_INET;
    server_address_ss1.sin_port=htons(receiver_info.ss_port);
    server_address_ss1.sin_addr.s_addr=inet_addr(receiver_info.ss_ip);
    int connect_status1=connect(sock_ss1,(struct sockaddr*)&server_address_ss1,sizeof(server_address_ss1));
    if(connect_status1==-1)
    {
        perror("Error in connect() function call 22: ");
        return -1;
    }
    char *msg_to_ss1=(char*)malloc(sizeof(char)*100);
    strcpy(msg_to_ss1,"copydir dest ");
    strcat(msg_to_ss1,dir_path);
    // strcat(msg_to_ss1,"/..");
    int i=strlen(msg_to_ss1)-1;
    while(msg_to_ss1[i]!='/')
    {
        i--;
    }
    // msg_to_ss1[i+1]='\0';
    // char *new_to_send=(char*)malloc(sizeof(char)*100);
    // int j=0;
    // for (j = 0; j < i; j++)
    // {
    //     new_to_send[j]=msg_to_ss1[j];
    // }
    // if (i==0)
    // {
    //     new_to_send[j]='/';
    //     j++;
    // }
    
    // new_to_send[j]='\0';

    msg_to_ss1[i+1]='\0';
    
    // msg_to_ss1[strlen(msg_to_ss1)]='\0';
    if(send(sock_ss1,msg_to_ss1,strlen(msg_to_ss1),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    usleep(1000);

    if(send(sock_ss1,&sender_info,sizeof(sender_info),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    int status;
    if(recv(sock_ss,&status,sizeof(status),0)==-1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }
    int status1;
    if(recv(sock_ss1,&status1,sizeof(status1),0)==-1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }
    close(sock_ss);
    close(sock_ss1);
    return 0;
}

int backup_file(ss_info sender_info1,ss_info reciever_info1,char *file_path)
{
    ss_info sender_info,reciever_info;
    sender_info.ss_port=sender_info1.ss_port;
    strcpy(sender_info.ss_ip,sender_info1.ss_ip);
    sender_info.s2s_port=sender_info1.s2s_port;
    sender_info.dir_or_file=sender_info1.dir_or_file;
    reciever_info.ss_port=reciever_info1.ss_port;
    strcpy(reciever_info.ss_ip,reciever_info1.ss_ip);
    reciever_info.s2s_port=reciever_info1.s2s_port;
    reciever_info.dir_or_file=reciever_info1.dir_or_file;
    sender_info.ss_ip[strlen(sender_info.ss_ip)]='\0';
    reciever_info.ss_ip[strlen(reciever_info.ss_ip)]='\0';


    int sock_ss=socket(AF_INET,SOCK_STREAM,0);
    if(sock_ss==-1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss;
    memset(&server_address_ss,0,sizeof(server_address_ss));
    server_address_ss.sin_family=AF_INET;
    server_address_ss.sin_port=htons(sender_info.ss_port);
    server_address_ss.sin_addr.s_addr=inet_addr(sender_info.ss_ip);
    int connect_status=connect(sock_ss,(struct sockaddr*)&server_address_ss,sizeof(server_address_ss));
    if(connect_status==-1)
    {
        perror("Error in connect() function call 11: ");
        return -1;
    }
    char *msg_to_ss=(char*)malloc(sizeof(char)*100);
    strcpy(msg_to_ss,"copyfile src ");
    strcat(msg_to_ss,file_path);
    msg_to_ss[strlen(msg_to_ss)]='\0';
    if(send(sock_ss,msg_to_ss,strlen(msg_to_ss),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    usleep(1000);

    if(send(sock_ss,&reciever_info,sizeof(reciever_info),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }

    int sock_ss1=socket(AF_INET,SOCK_STREAM,0);
    if(sock_ss1==-1)
    {
        perror("Error in socket() function call: ");
        return -1;
    }
    struct sockaddr_in server_address_ss1;
    memset(&server_address_ss1,0,sizeof(server_address_ss1));
    server_address_ss1.sin_family=AF_INET;
    server_address_ss1.sin_port=htons(reciever_info.ss_port);
    server_address_ss1.sin_addr.s_addr=inet_addr(reciever_info.ss_ip);
    int connect_status1=connect(sock_ss1,(struct sockaddr*)&server_address_ss1,sizeof(server_address_ss1));
    if(connect_status1==-1)
    {
        perror("Error in connect() function call 22: ");
        return -1;
    }
    char *msg_to_ss1=(char*)malloc(sizeof(char)*100);
    strcpy(msg_to_ss1,"copyfile dest ");
    strcat(msg_to_ss1,file_path);
    // strcat(msg_to_ss1,"/..");
    int i=strlen(msg_to_ss1)-1;
    while(msg_to_ss1[i]!='/')
    {
        i--;
    }
    msg_to_ss1[i+1]='\0';
    // msg_to_ss1[strlen(msg_to_ss1)]='\0';
    if(send(sock_ss1,msg_to_ss1,strlen(msg_to_ss1),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    usleep(1000);

    if(send(sock_ss1,&sender_info,sizeof(sender_info),0)==-1)
    {
        perror("Error in send() function call: ");
        return -1;
    }
    int status;
    if(recv(sock_ss,&status,sizeof(status),0)==-1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }
    int status1;
    if(recv(sock_ss1,&status1,sizeof(status1),0)==-1)
    {
        perror("Error in recv() function call: ");
        return -1;
    }
    close(sock_ss);
    close(sock_ss1);
}