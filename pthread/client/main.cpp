#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>


#define MAX_DATA_SIZE 2048
#define PORT 8000
#define BACKLOG 10
#define LISTENQ 20
#define MAX_CONNECT 20

void *recv_message(void *fd)
{
    int sockfd = *(int *)fd;
    while (true)
    {
        char buf[MAX_DATA_SIZE];
        memset(buf,0,MAX_DATA_SIZE);
        ssize_t n;
        n = recv(sockfd,buf,MAX_DATA_SIZE,0);
        if(n < 0)
        {
            printf("recv error\n");
            exit(1);
        }

        buf[n] = '\0';
        if(strcmp(buf,"byebye.") == 0)
        {
            printf("Server close\n");
            close(sockfd);
            exit(0);
        }

        printf("Server: %s\n",buf);
    }
}

int main()
{
    int sockfd;
    pthread_t recv_tid, send_tid;

    sockaddr_in serveraddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        printf("socket error\n");
        exit(1);
    }

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family =AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.90.34");

    if(connect(sockfd,(struct sockaddr *)&serveraddr, sizeof (serveraddr)) < 0)
    {
        printf("create error\n");
        exit(1);
    }


    //create thread
    if(pthread_create(&recv_tid,nullptr,recv_message,&sockfd) < 0)
    {
        printf("create thread error\n");
        exit(1);
    }

    char msg[MAX_DATA_SIZE];
    memset(msg,0,MAX_DATA_SIZE);
    while (fgets(msg,MAX_DATA_SIZE,stdin) != nullptr)
    {
        if(strcmp(msg,"exit\n") == 0)
        {
            printf("byebye.\n");
            memset(msg,0,MAX_DATA_SIZE);
            strcpy(msg,"byebye.");
            send(sockfd,msg,strlen(msg),0);
            close(sockfd);
            exit(0);
        }

        if(send(sockfd,msg,strlen(msg),0) < 0)
        {
            printf("send error\n");
            exit(1);
        }
    }


    return 0;
}
