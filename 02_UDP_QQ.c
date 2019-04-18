#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

void *deal_recv_msg(void *arg)
{
    int sockfd = *(int *)arg;
    struct sockaddr_in fromAddr;
    socklen_t addrLen = sizeof(fromAddr);
    char ip_str[16] = "";

    while(1)
    {
    	unsigned char buf[1024] = "0";
    	recvfrom(sockfd, buf, sizeof(buf), 0, 
    			(struct sockaddr *)&fromAddr, &addrLen);
    	inet_ntop(AF_INET, &fromAddr.sin_addr.s_addr, 
    			ip_str, INET_ADDRSTRLEN);
    	printf("\r[%s:%hu] %s\n", ip_str, 
    			ntohs(fromAddr.sin_port),buf);
    	printf("\r[say:]");
    	fflush(NULL);
    }
}

int main(int argc, char *argv[])
{
    if(argc<2)
    {
        printf("please input like this: ./demo 8000\n");
        return 0;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("socket");
        return 0;
    }
    
    struct sockaddr_in hostAddr;
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(atoi(argv[1]));
    hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr *)&hostAddr, sizeof(hostAddr));

    pthread_t tid;
    pthread_create(&tid, NULL, deal_recv_msg, (void *)&sockfd);
    pthread_detach(tid);

    struct sockaddr_in dstAddr;
    bzero(&dstAddr, sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;

    while(1)
    {
    	printf("\r[say:]");
    	fflush(NULL);
    	char msg[128] = "0";
    	fgets(msg, sizeof(msg), stdin);
    	msg[strlen(msg)-1] = 0;

    	if(strncmp(msg,"sayto", 5) == 0)
    	{
    		char ip_str[16] = "";
    		unsigned short port = 0;
    		sscanf(msg,"sayto %s %hu",ip_str, &port);

    		inet_pton(AF_INET, ip_str, &dstAddr.sin_addr.s_addr);
    		dstAddr.sin_port = htons(port);
    		continue;
    	}

    	sendto(sockfd, msg, sizeof(msg), 0,
    		(struct sockaddr *)&dstAddr, sizeof(dstAddr));

    }
    close(sockfd);
    return 0;
}