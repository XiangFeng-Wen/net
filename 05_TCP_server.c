#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc,char *argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("sockfd");
        _exit(-1);
    }

    struct sockaddr_in host_addr;
    bzero(&host_addr, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(8080);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr));

    listen(sockfd, 8);

    int i = 0;

    while (i<5)
    {
        i++;
        struct sockaddr_in new_addr;
        socklen_t addrlen = sizeof(new_addr);
        int newfd = accept(sockfd, (struct sockaddr *)&new_addr, &addrlen);
        
        unsigned char buf[512] = "";
        char ip[16] = "";
        inet_ntop(AF_INET, &new_addr.sin_addr.s_addr, ip, 16);
        printf("[%s:%hu]已连接\n",ip, ntohs(new_addr.sin_port));

        int len = recv(newfd, buf, sizeof(buf), 0);
        printf("buf = %s\n",buf);

        send(newfd, buf, len, 0);

        close(newfd);
        printf("第%d个客户端已处理\n",i);
    }
    close(sockfd);
    printf("所有客户端处理完毕\n");
    return 0;
}