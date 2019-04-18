#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>


int main(int argc, char const *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("create socket");
        return 0;
    }

    struct sockaddr_in dest_addr;
    bzero(&dest_addr,sizeof(dest_addr));
    
    char *msg = "hello, good to see you";

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "10.0.122.178", &dest_addr.sin_addr.s_addr);

    sendto(sockfd, msg, strlen(msg), 0,
          (struct sockaddr *)&dest_addr,
           sizeof(dest_addr));

    close(sockfd);
    
    return 0;
}
