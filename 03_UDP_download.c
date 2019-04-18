#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("please input like this: ./demo 10.0.122.178 a.txt\n");
		return 0;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
        return 0;
	}

	char sendCmd[1024] = "0";
	int len = sprintf(sendCmd, "%c%c%s%c%s%c", 
					0,1, argv[2], 0,"octet", 0);

	struct sockaddr_in dstAddr;
    bzero(&dstAddr,sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(69);
    inet_pton(AF_INET, argv[1], &dstAddr.sin_addr.s_addr);

    sendto(sockfd, sendCmd, len, 0, 
    	(struct sockaddr *)&dstAddr,
        sizeof(dstAddr));

    int fd = open(argv[2], O_WRONLY|O_CREAT, 0777);
    if (fd < 0)
    {
    	perror("open");
    }

    struct sockaddr_in fromAddr;
    socklen_t addrLen = sizeof(fromAddr);
    while(1)
    {
    	char buf[516] = "0";
    	int ret = recvfrom(sockfd, buf, sizeof(buf), 0,
    					(struct sockaddr *)&fromAddr, &addrLen);
    	printf("ret = %d\n", ret);
    	if (buf[1] == 3)
    	{
    		int recvLen = write(fd, buf+4, ret-4);
    		buf[1] = 4;
    		buf[4] = 0;
    	
    		sendto(sockfd, buf, 4, 0, 
    			(struct sockaddr *)&fromAddr,addrLen);

    		unsigned short num;
    		sprintf((char *)&num, "%c%c", buf[2], buf[3]);
    		
    		printf("已收到第%hu个包\n",num);

    		if (recvLen < 512)
	    		break;
    	}
    }
    printf("下载完毕\n");

    close(sockfd);
    close(fd);
	return 0;
}
