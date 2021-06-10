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
            printf("client close\n");
            close(sockfd);
            exit(1);
        }

        printf("\nClient: %s\n",buf);
    }
}


int main()
{
    int listenfd, connfd;
    socklen_t clilen;

    pthread_t recv_tid, send_tid;

    struct sockaddr_in serveraddr, clientaddr;


    listenfd = socket(AF_INET, SOCK_STREAM,0);
    if(listenfd < 0)
    {
        printf("socket error\n");
        exit(1);
    }

    bzero(&serveraddr,sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);


    if(bind(listenfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
       printf("bind error\n");
       exit(1);
    }

    if(listen(listenfd,LISTENQ) < 0)
    {
        printf("listen error\n");
        exit(1);
    }

    clilen = sizeof (clientaddr);
    connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clilen);
    if(connfd < 0)
    {
        printf("accept error\n");
        exit(1);
    }

    printf("accept a client: %s:%d\n",inet_ntoa(clientaddr.sin_addr),clientaddr.sin_port);

    //create thread
    if(pthread_create(&recv_tid,nullptr,recv_message,&connfd) == -1)
    {
        printf("thread create error\n");
        exit(1);
    }


    char msg[MAX_DATA_SIZE];
    memset(msg,0,MAX_DATA_SIZE);
    while (fgets(msg,MAX_DATA_SIZE,stdin) != nullptr)
    {
        if(strcmp(msg,"exit\n") == 0)
        {
            printf("byebye\n");
            memset(msg,0,MAX_DATA_SIZE);
            strcpy(msg,"byebye.");
            send(connfd,msg,strlen(msg),0);
            close(connfd);
            exit(0);
        }

        if(send(connfd,msg,strlen(msg),0) < 0)
        {
            printf("send error\n");
            exit(1);
        }
    }

    return 0;
}
