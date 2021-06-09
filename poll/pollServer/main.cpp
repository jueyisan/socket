#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>


#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif

#ifndef INFTIM
#define INFTIM -1
#endif

#define PORT 8000
#define MAX_DATA_SIZE 2048
#define LISTENQ 20


int main()
{
    int i, maxi, listenfd, connfd, sockfd;
    int nready;

    ssize_t n, ret;
    struct pollfd client[OPEN_MAX];

    char buf[MAX_DATA_SIZE];

    socklen_t clilen;

    struct sockaddr_in serveraddr, clientaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof (serveraddr));

    listen(listenfd,LISTENQ);

    //set poll
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for (i = 1; i < OPEN_MAX; i++) {
        client[i].fd = -1;
    }
    maxi = 0;

    while (true) {
        nready = poll(client, maxi + 1, INFTIM);

        if(client[0].revents & POLLRDNORM)
        {
            clilen = sizeof (clientaddr);
            printf("accept connection\n");

            connfd = accept(listenfd,(struct sockaddr *)&clientaddr, &clilen);
            if(connfd < 0)
            {
                printf("accept error\n");
                exit(1);
            }

            printf("accept a new client: %s:%d\n",inet_ntoa(clientaddr.sin_addr),clientaddr.sin_port);

            for(i = 1; i < OPEN_MAX; i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }

            if(OPEN_MAX == i)
            {
                printf("too many connection\n");
                exit(1);
            }

            client[i].events = POLLRDNORM;
            if(i > maxi)
                maxi = i;

            if(--nready < 0)
                continue;
        }

        for (i = 1; i <= maxi; i++) {
            if((sockfd = client[i].fd) < 0)
                continue;

            if(client[i].revents & (POLLRDNORM | POLLERR))
            {
                printf("read the socket\n");

                bzero(buf, MAX_DATA_SIZE);
                n = read(sockfd,buf,sizeof (buf) -1);
                if(n <= 0)
                {
                    close(sockfd);
                    client[i].fd = -1;
                }
                else {
                    printf("client[%d] send message: %s\n",i,buf);
                    for (int j = 0; j < strlen(buf);j++) {
                        buf[j] = toupper(buf[j]);
                    }
                    ret = write(sockfd,buf,strlen(buf));
                    if(ret != n)
                    {
                        printf("write error\n");
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
