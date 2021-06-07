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


ssize_t readline(int fd, char *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1 ; n < maxlen; n++) {
        if((rc == read(fd,&c,1)) == 1)
        {
            *ptr++ = c;
            if(c == '\n')
                break;
        }
        else if (rc == 0) {
            *ptr = 0;
            return (n-1);
        }
        else {
            return -1;
        }
    }

    *ptr = 0;
    return (n);
}


int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        printf("create socket error\n");
        exit(1);
    }

    bzero(&serverAddr,sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET,"192.168.90.34",&serverAddr.sin_addr) < 0)
    {
        printf("inet_pton error\n");
        exit(1);
    }

    if(connect(sockfd,(struct sockaddr *)&serverAddr,sizeof (serverAddr)) < 0)
    {
        printf("connect error\n");
        exit(1);
    }

    char sendLine[MAXDATASIZE],recvLine[MAXDATASIZE];
    while(fgets(sendLine,MAXDATASIZE,stdin) != nullptr)
    {
        write(sockfd,sendLine,strlen(sendLine));

        if(readline(sockfd,recvLine,MAXDATASIZE) == 0)
        {
            printf("server terminated prematurely\n");
            exit(1);
        }

        if(fputs(recvLine,stdout) == EOF)
        {
            printf("fputs error\n");
            exit(1);
        }
    }

    close(sockfd);

    return 0;
}
