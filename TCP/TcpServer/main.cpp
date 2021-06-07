#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXDATASIZE 2048
#define PORT 8000
#define BACKLOG 10
#define LISTENQ 6666
#define MAXCONNECT 20

int main()
{
    struct sockaddr_in serverAddr, clientAddr;

    int listenfd, connfd;
    pid_t childpid;

    char buf[MAXDATASIZE];

    socklen_t clilen;

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd < 0)
    {
        printf("listen socket error\n");
        exit(1);
    }

    bzero(&serverAddr, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    int ret = bind(listenfd,(struct sockaddr *)&serverAddr,sizeof (serverAddr));
    if(ret < 0)
    {
        printf("bind error\n");
        exit(1);
    }

    if(listen(listenfd,LISTENQ) < 0)
    {
        printf("listen error\n");
        exit(1);
    }

    while (true) {
        clilen = sizeof (clientAddr);
        connfd = accept(listenfd,(struct sockaddr *)&clientAddr,&clilen);
        if(connfd < 0)
        {
            printf("accept error\n");
            exit(1);
        }
        printf("aceept success\n");
        printf("IP = %s:PORT = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        ssize_t n;
        char msg[MAXDATASIZE];
        while ((n = read(connfd,msg,MAXDATASIZE)) > 0) {
            write(connfd,msg,n);
        }
        close(connfd);
    }

    close(listenfd);

    return 0;
}
