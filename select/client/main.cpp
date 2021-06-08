#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>


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
        if(read(sockfd,recvline,sizeof (recvline) -1) < 0)
        {
            printf("server terminated prematurel\n");
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


void select_cli(FILE* fp, int sockfd)
{
    int maxfd;
    fd_set rset;

    char sendline[MAX_DATA_SIZE], recvline[MAX_DATA_SIZE];

    FD_ZERO(&rset);
    while (true) {
        FD_SET(fileno(fp),&rset);
        FD_SET(sockfd,&rset);
        maxfd = max(fileno(fp),sockfd) + 1;
        select(maxfd, &rset, nullptr,nullptr,nullptr);

        if(FD_ISSET(fileno(fp),&rset))
        {
            if(fgets(sendline,MAX_DATA_SIZE,fp) == nullptr)
            {
                printf("read nothing\n");
                close(sockfd);
                return ;
            }
            sendline[strlen(sendline) - 1] = '\0';
            write(sockfd,sendline,strlen(sendline));
        }

        if(FD_ISSET(sockfd,&rset))
        {
            if(read(sockfd,recvline,sizeof (recvline) -1) == 0)
            {
                printf("handleMsg: server terminated prematurel\n");
                exit(1);
            }

            if(fputs(recvline,stdout) == EOF)
            {
                printf("fputs error\n");
                exit(1);
            }
        }
    }
}


int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        printf("init socket error\n");
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

    str_cli(sockfd);
    //select_cli(stdin,sockfd);

    return 0;
}
