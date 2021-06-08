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

    int listenfd, connfd, sockfd;

    char buf[MAX_DATA_SIZE];
    char msg[] = "hello world";

    socklen_t clilen;
    pid_t chilpid;

    struct sockaddr_in serverAddr, clientAddr;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&serverAddr, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    bind(listenfd,(struct sockaddr *)&serverAddr,sizeof (serverAddr));

    listen(listenfd,LISTENQ);

    //start server
    printf("start server listen\n");
    while(true)
    {
        clilen = sizeof (clientAddr);
        connfd = accept(listenfd,(struct sockaddr *)&clientAddr,&clilen);
        if(connfd < 0)
        {
            printf("accept error\n");
            exit(1);
        }
        printf("aceept success\n");
        printf("IP = %s:PORT = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


        if((chilpid = fork()) == 0)
        {
            close(listenfd);


            //write(connfd,msg,strlen(msg));

            while(read(connfd,buf,sizeof (buf)-1) > 0)
            {
                printf("client recv: %s\n",buf);
                write(connfd,buf,strlen(buf));
            }
            exit(0);
        }

        close(connfd);
    }

    close(listenfd);

    return 0;
}
