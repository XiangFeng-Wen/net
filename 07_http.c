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

char err[]=	"HTTP/1.1 404 Not Found\r\n"		\
		"Content-Type: text/html\r\n"		\
		"\r\n"								\
		"<HTML><BODY>File not found</BODY></HTML>";

char head[]="HTTP/1.1 200 OK\r\n"					\
			"Content-Type: text/html\r\n"		\
			"\r\n";

void *deal_pthread(void *arg)
{
    usleep(10);
    int newfd = (int)arg;
    
    unsigned char buf[1024] = "";
    int len = recv(newfd, buf, sizeof(buf), 0);
    if (len == 0)
    {
        perror("recv");
        close(newfd);
        return;
    }
    printf("buf = %s\n",buf);

    strtok(buf," ");
    char fileName[128] = "0";
    sprintf(fileName, "./html%s", strtok(NULL," "));
    if (fileName[7] == 0)
    {
        printf("fileName[7] == 0\n");
        bzero(&fileName,sizeof(fileName));
        strcpy(fileName,"./html/index.html");
    }
    printf("fileName = %s\n",fileName);

    int fd = open(fileName, O_RDONLY);
    if(fd < 0)
    {
        send(newfd, err, strlen(err), 0);
        perror("open");
    }
    else
    {
        send(newfd, head, strlen(head), 0);
        while(1)
        {
            bzero(&buf,sizeof(buf));
            len = read(fd, buf, sizeof(buf));
            printf("buf_len = %d,sizeof(buf) = %d\n", len,sizeof(buf));
            if (len > 0)
                send(newfd, buf, len, 0);
            else if(len < 0)
            {
                perror("read");
                break;
            }
            
            //有符号和无符号进行比较会把有符号自动转成无符号变量
            if(len < sizeof(buf))
            {
                printf("发送成功\n");
                break;
            }          
        } 
        close(fd);     
    }
    
    close(newfd);
    printf("线程结束\n");

    return; 
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("please input:./demo port\n");
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("sockfd");
        return -1;
    }

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

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
