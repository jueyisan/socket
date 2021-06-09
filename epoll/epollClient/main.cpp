#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <time.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 8000
#define MAX_DATA_SIZE 2048

int max(int a, int b)
{
    return a > b ? a : b;
}

void str_cli(int sockfd)
{
    char sendline[MAX_DATA_SIZE], recvline[MAX_DATA_SIZE];
    while(fgets(sendline,MAX_DATA_SIZE,stdin) != nullptr)
    {
        write(sockfd,sendline,strlen(sendline));

        bzero(recvline,MAX_DATA_SIZE);
        if(read(sockfd,recvline,sizeof (recvline)-1) == 0)
        {
            printf("server error\n");
            exit(1);
        }

        if(fputs(recvline,stdout) == EOF)
        {
            printf("fputs error\n");
            exit(1);
        }

        bzero(sendline,MAX_DATA_SIZE);

    }
}

int main()
{
    int sockfd;
    struct sockaddr_in serveraddr;

    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd < 0)
    {
        printf("socket error\n");
        exit(1);
    }

    bzero(&serveraddr,sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.90.34");

    if(connect(sockfd,(struct sockaddr *)&serveraddr, sizeof (serveraddr)) < 0)
    {
        printf("connect error\n");
        exit(1);
    }

    str_cli(sockfd);

    return 0;
}
