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
        if(mac_type == 0x0800)
        {
            printf("IPv4_DATA\n");
            char *ip_buf = buf+14;
            int head_len = (*ip_buf & 0x0f) * 4;
            printf("IP报文头长度：%d\n",head_len);
            printf("IP报文总长度 = %hu\n",ntohs(*(unsigned short *)(ip_buf+2)));
            unsigned char ip_type = *(unsigned char *)(ip_buf + 9);
            unsigned char src_addr[16] = "";
            unsigned char dst_addr[16] = "";
            inet_ntop(AF_INET, (unsigned int *)(ip_buf + 12), src_addr,16);
            inet_ntop(AF_INET, (unsigned int *)(ip_buf + 16), dst_addr,16);
            printf("IPv4:%s --> %s\n",src_addr,dst_addr);
            printf("ip_type = %d\n",ip_type);
            if (ip_type == 1)
            {
                printf("ICMP协议\n");
            }else if(ip_type == 2)
            {
                printf("IGMP协议\n");
            }else if(ip_type == 6)
            {
                printf("TCP协议\n");
                unsigned char *tcp_buf = ip_buf + head_len;
                unsigned short src_port = ntohs(*(unsigned short *)tcp_buf);
                unsigned short dst_port = ntohs(*(unsigned short *)(tcp_buf+2));
                printf("port:%hu --> %hu\n",src_port,dst_port);
                unsigned char data_head_len = *(unsigned char *)(tcp_buf+12);
                data_head_len = (data_head_len >> 4) * 4;
                printf("TCP头部长度 = %d\n",data_head_len);
                printf("TCP数据：%s\n",tcp_buf + data_head_len);

                break;

                
            }else if(ip_type == 17)
            {
                printf("UDP协议\n");
                char *udp_buf = ip_buf + head_len;
                unsigned short src_port = ntohs(*(unsigned short *)udp_buf);
                unsigned short dst_port = ntohs(*(unsigned short *)(udp_buf+2));
                printf("port:%hu --> %hu\n",src_port,dst_port);
                unsigned short data_len = ntohs(*(unsigned short *)(udp_buf+4));
                printf("UDP数据包长度 = %hu\n",data_len);
                printf("UDP数据：%s\n",udp_buf+8);
                if (src_port == 8080)
                {
                    break;
                }
            }
        }
        else if(mac_type == 0x0806)
        {
            printf("ARP_DATA\n");
        }
        else if(mac_type == 0x8035)
        {
            printf("RARP_DATA\n");
        }
        else if(mac_type == 0x86dd)
        {
            printf("IPv6_DATA\n");
        }
        else
        {
            printf("Undefined Type\n");
        }
        puts("");
    }
    
    return 0 ;
}

