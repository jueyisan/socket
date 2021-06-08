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
#define LISTENQ 20


int main()
{

    int i, maxi, maxfd, listenfd, confd, sockfd;
    int nready, client[FD_SETSIZE];

    ssize_t n, ret;
    fd_set rset, allset;

    char buf[MAX_DATA_SIZE];

    socklen_t clilen;

    struct sockaddr_in serverAddr, clientAddr;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&serverAddr, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    bind(listenfd,(struct sockaddr *)&serverAddr,sizeof (serverAddr));

    listen(listenfd,LISTENQ);

    //set select
    maxfd = listenfd;
    maxi = -1;
    for(i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);

    //start server
    printf("start server listen\n");
    while(true)
    {
        rset = allset;
        nready = select(maxfd+1,&rset,nullptr,nullptr,nullptr);

        if(FD_ISSET(listenfd,&rset))
        {
            clilen = sizeof (clientAddr);

            printf("accept connection\n");
            confd = accept(listenfd,(struct sockaddr *)&clientAddr,&clilen);
            if( confd < 0)
            {
                printf("accept error\n");
                exit(1);
            }

            printf("accept a new client: %s:%d \n",inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);

            // put client socket into client array
            for (i = 0; i < FD_SETSIZE; i++) {
                if(client[i] < 0)
                {
                    client[i] = confd;
                    break;
                }
            }

            if(FD_SETSIZE == i)
            {
                printf("too many connection\n");
                exit(1);
            }

            FD_SET(confd,&allset);
            if(confd > maxfd)
                maxfd = confd;
            if(i > maxi)
                maxi = i;

            if(--nready < 0)
                continue;
        }

        for (i = 0; i < maxi; i++) {
            if((sockfd = client[i]) < 0)
                continue;

            if(FD_ISSET(sockfd,&rset))
            {
                printf("reading the socket\n");

                bzero(buf,MAX_DATA_SIZE);
                if((n = read(sockfd,buf,MAX_DATA_SIZE)) <= 0)
                {
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }
                else {
                    printf("client[%d] send message: %s\n",i,buf);
                    if((ret = write(sockfd,buf,n)) != n)
                    {
                        printf("error write to the socket\n");
                        break;
                    }
                }

                if(--nready <= 0)
                    break;

            }
        }
    }

    return 0;
}
