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

#ifndef CONNECT_SIZE
#define CONNECT_SIZE 256
#endif

#define PORT 8000
#define MAX_DATA_SIZE 2048
#define LISTENQ 20

void setNonblocking(int sockfd)
{
    int opts;
    opts = fcntl(sockfd,F_GETFL);
    if(opts < 0)
    {
        printf("fcntl(sockfd,F_GETFL)\n");
        return;
    }

    opts = opts | O_NONBLOCK;
    if(fcntl(sockfd,F_SETFL,opts) < 0)
    {
        printf("fcntl(sockfd,F_SETFL,opts)\n");
        return;
    }
}


int main()
{

    int i , listenfd, connfd, sockfd, epfd, nfds;

    ssize_t n, ret;

    char buf[MAX_DATA_SIZE];

    socklen_t clilen;

    struct sockaddr_in serveraddr, clientaddr;

    struct epoll_event ev, events[20];

    listenfd = socket(AF_INET,SOCK_STREAM, 0);
    setNonblocking(listenfd);

    epfd = epoll_create(CONNECT_SIZE);
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    bzero(&serveraddr,sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    bind(listenfd, (struct sockaddr *)&serveraddr, sizeof (serveraddr));

    listen(listenfd,LISTENQ);

    while(true)
    {
        nfds = epoll_wait(epfd,events,CONNECT_SIZE,-1);
        if(nfds <= 0)
        {
            continue;
        }

        printf("nfds = %d \n",nfds);

        for (i = 0; i < nfds; i++) {
            if(events[i].data.fd == listenfd)
            {
                clilen = sizeof(clientaddr);
                connfd = accept(listenfd,(struct sockaddr *)&clientaddr, &clilen);
                if(connfd < 0)
                {
                    printf("accept error\n");
                    exit(1);
                }

                printf("accept a new client: %s:%d",inet_ntoa(clientaddr.sin_addr),clientaddr.sin_port);

                setNonblocking(connfd);
                ev.data.fd = connfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd, &ev);
            }
            else if (events[i].events & EPOLLIN) {
                if((sockfd = events[i].data.fd) < 0)
                    continue;
                bzero(buf,MAX_DATA_SIZE);
                printf("read the socket\n");
                n = read(sockfd,buf,sizeof (buf)-1);
                if(n <= 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                else {
                    buf[n] = '\0';
                    printf("client[%d] send message: %s\n",i,buf);
                    ev.data.fd = sockfd;
                    ev.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
                }
            }
            else if (events[i].events & EPOLLOUT) {
                if((sockfd = events[i].data.fd) < 0)
                {
                    continue;
                }

                if((ret = write(sockfd, buf, strlen(buf))) != n)
                {
                    printf("error write\n");
                    break;
                }

                ev.data.fd = sockfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }

    free(events);
    close(epfd);

    return 0;
}
