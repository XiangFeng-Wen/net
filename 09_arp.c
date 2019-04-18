#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

int main(int argc,char *argv[])
{
    int sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    while(1){
        unsigned char buf[2048] = "";
        int len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
        if(len < 0)
        {
            perror("recvfrom");
            return -1;
        }

        char dst_mac[18] = "";
        char src_mac[18] = "";
        sprintf(dst_mac, "%02x:%02x:%02x:%02x:%02x:%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
        sprintf(src_mac, "%02x:%02x:%02x:%02x:%02x:%02x", buf[0+6], buf[1+6], buf[2+6], buf[3+6], buf[4+6], buf[5+6]);
        printf("mac: %s --> %s\n", src_mac, dst_mac);
        unsigned short mac_type = ntohs(*(unsigned short *)(buf + 12));
        printf("mac_type = %0#x\n",mac_type);
        if(mac_type == 0x0806)
        {
            printf("ARP_DATA\n");
        }
        puts("");
    }
    
    return 0 ;
}

