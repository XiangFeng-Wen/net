#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in host_addr;
    bzero(&host_addr,sizeof(host_addr));

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(8080);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd,(struct sockaddr *)&host_addr, sizeof(host_addr));

    struct sockaddr_in dest_addr;
    socklen_t d_addr_len = sizeof(dest_addr);

    unsigned char buf[128] = "0";
    recvfrom(sockfd, buf, sizeof(buf), 0, 
            (struct sockaddr *)&dest_addr, &d_addr_len);

    char dest_ip[16] = "0";
    inet_ntop(AF_INET, &dest_addr.sin_addr.s_addr, 
              dest_ip, INET_ADDRSTRLEN);

    printf("buf = %s\n",buf);

    sendto(sockfd, "recerved", strlen("recerved"), 0, (struct sockaddr *)&dest_addr, d_addr_len);

    close(sockfd);
    return 0;
}
