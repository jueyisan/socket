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
#include <ctype.h>


#define PORT 8000
#define MAX_DATA_SIZE 2048
#define LISTENQ 20


int main()
{

    int i, maxi, maxfd, listenfd, connfd, sockfd;

    int nready , client[FD_SETSIZE];

    ssize_t n, ret;

    fd_set rset , allset;

    char buf[MAX_DATA_SIZE];

    socklen_t clilen;

    struct sockaddr_in servaddr , cliaddr;

    /*(1) 得到监听描述符*/
    listenfd = socket(AF_INET , SOCK_STREAM , 0);

    /*(2) 绑定套接字*/
    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr));

    /*(3) 监听*/
    listen(listenfd , LISTENQ);

    /*(4) 设置select*/
    maxfd = listenfd;
    maxi = -1;
    for(i=0 ; i<FD_SETSIZE ; ++i)
    {
        client[i] = -1;
    }//for
    FD_ZERO(&allset);
    FD_SET(listenfd , &allset);

    printf("ready to listen~~~ \n");
    /*(5) 进入服务器接收请求死循环*/
    while(1)
    {
        rset = allset;
        nready = select(maxfd+1 , &rset , nullptr , nullptr , nullptr);

        if(FD_ISSET(listenfd , &rset))
        {
            /*接收客户端的请求*/
            clilen = sizeof(cliaddr);

            printf("accpet connection~\n");

            if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
            {
                perror("accept error.\n");
                exit(1);
            }

            printf("accpet a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr) , cliaddr.sin_port);

            /*将客户链接套接字描述符添加到数组*/
            for(i=0 ; i < FD_SETSIZE ; ++i)
            {
                if(client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if(FD_SETSIZE == i)
            {
                perror("too many connection.\n");
                exit(1);
            }

            FD_SET(connfd , &allset);
            if(connfd > maxfd)
                maxfd = connfd;
            if(i > maxi)
                maxi = i;

            if(--nready < 0)
                continue;
        }

        for(i = 0; i <= maxi ; ++i)
        {
            if((sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd , &rset))
            {
                /*处理客户请求*/
                printf("reading the socket~~~ \n");

                bzero(buf , MAX_DATA_SIZE);
                if((n = read(sockfd , buf , sizeof (buf) - 1)) <= 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd , &allset);
                    client[i] = -1;
                }
                else{
                    printf("clint[%d] send message: %s\n", i , buf);
                    for (int j = 0; j < n; j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    if((ret = write(sockfd , buf , n)) != n)
                    {
                        printf("error writing to the sockfd!\n");
                        break;
                    }
                }

                if(--nready <= 0)
                    break;
            }
        }
    }
}
