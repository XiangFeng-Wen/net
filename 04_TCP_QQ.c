#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in dstAddr;
    bzero(&dstAddr,sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(8080);
    dstAddr.sin_addr.s_addr = inet_addr("10.0.122.178");

    connect(sockfd, (struct sockaddr *)&dstAddr, sizeof(dstAddr));

    char msg[] = "hello";
    unsigned char buf[128] = "";
    send(sockfd, msg, strlen(msg), 0);
    
    recv(sockfd, buf, sizeof(buf), 0);
    printf("buf = %s\n",buf);
    close(sockfd);   
    return 0;
}
