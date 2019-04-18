#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

void *deal_pthread(void *arg)
{
    int fd = (int)arg;
    while(1)
    {
        unsigned char buf[1024] = "";
        int len = recv(fd, buf, sizeof(buf), 0);

        if (len == 0)
            break;
        send(fd, buf, len, 0);
    }

    close(fd);
    return;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("please input:./demo port\n");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("sockfd");
        return -1;
    }

    struct sockaddr_in hostAddr;
    bzero(&hostAddr, sizeof(hostAddr));
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(atoi(argv[1]));
    hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(sockfd, (struct sockaddr *)&hostAddr, sizeof(hostAddr));
    if(ret != 0)
    {
        perror("bind");
        return -1;
    }

    ret = listen(sockfd, 15);
    if (ret != 0)
    {
        perror("listen");
        return -1;
    }

    while(1)
    {
        struct sockaddr_in newAddr;
        int addrLen = sizeof(newAddr);
        char ip[16] = "";
        int newfd = accept(sockfd, (struct sockaddr *)&newAddr, &addrLen);    
        if (newfd < 0)
        {
            perror("accept");
            return -1;
        }

        inet_ntop(AF_INET, &newAddr.sin_addr.s_addr, ip, 16);
        printf("[%s:%hu]连接到服务器\n", ip, ntohs(newAddr.sin_port));

        pthread_t tid;
        pthread_create(&tid,NULL,deal_pthread,(void *)newfd);
        pthread_detach(tid);
    }
    

    close(sockfd);
    return 0;
}
