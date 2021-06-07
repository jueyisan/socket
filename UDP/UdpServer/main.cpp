#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8000
#define MAXDATASIZE 2048

int main()
{
    int sockfd;
    struct sockaddr_in serverAddr;

    // create serveraddr
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;    //IPv4
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // create socket
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if( sockfd < 0)
    {
        printf("socket error\n");
        return -1;
    }

    // bind port
    if(bind(sockfd,(struct sockaddr *)&serverAddr, sizeof (serverAddr)))
    {
        printf("bing error\n");
        return -1;
    }

    int n;
    socklen_t len;
    struct sockaddr_in connectAddr;
    char buf[MAXDATASIZE];

    while(true)
    {
        len = sizeof (connectAddr);
        n = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&connectAddr, &len);
        if(n < 0)
        {
            printf("recvfrom error\n");
            return -1;
        }
        printf("recvfrom success %d\n",n);
        printf("IP = %s:PORT = %d\n", inet_ntoa(connectAddr.sin_addr), ntohs(connectAddr.sin_port));

        n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&connectAddr, len);
        if(n < 0)
        {
            printf("sendto error\n");
            return -1;
        }
        printf("sendto success %d\n",n);

    }
}
