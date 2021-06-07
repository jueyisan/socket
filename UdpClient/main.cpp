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
    int sockfd,t;
    struct sockaddr_in serverAddr;

    bzero(&serverAddr,sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    t = inet_pton(AF_INET, "192.168.90.34",&serverAddr.sin_addr);
    if(t <= 0)
    {
        printf("inet_pton error\n");
        return -1;
    }

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        printf("socket error\n");
        return -1;
    }

    int n;
    char sendLine[MAXDATASIZE], recvLine[MAXDATASIZE+1];

    while (fgets(sendLine,MAXDATASIZE,stdin) != NULL)
    {
        n = sendto(sockfd,sendLine,strlen(sendLine),0,(struct sockaddr *)&serverAddr, sizeof (serverAddr));
        if(n < 0)
        {
            printf("sendto error\n");
            return -1;
        }

        n = recvfrom(sockfd,recvLine,MAXDATASIZE,0,NULL,NULL);
        if(n < 0)
        {
            printf("recvfrom error\n");
            return -1;
        }

        recvLine[n] = '\0';
        fputs(recvLine,stdout);
    }


    return 0;
}
