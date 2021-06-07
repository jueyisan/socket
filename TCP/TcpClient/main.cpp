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
    serverAddr.sin_addr.s_addr = inet_addr("192.168.90.34");

    if(connect(sockfd,(struct sockaddr *)&serverAddr,sizeof (serverAddr)) < 0)
    {
        printf("connect error\n");
        exit(1);
    }

    char sendLine[MAXDATASIZE],recvLine[MAXDATASIZE];
    if(fgets(sendLine,MAXDATASIZE,stdin) != nullptr)
    {
        write(sockfd,sendLine,strlen(sendLine));
    }

    while (true) {
        int len = read(sockfd,recvLine,sizeof (recvLine) -1);
        if(len < 0)
        {
            printf("read error\n");
        }

        if(fputs(recvLine,stdout) == EOF)
        {
            printf("fputs error\n");
            exit(1);
        }

        break;
    }

    close(sockfd);

    return 0;
}
