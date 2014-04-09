/******************************************
 * Imie i nazwisko : Marek Jenda          *
 * Nr indeksu      : 241647               *
 * Nazwa programu  : traceroute           *
 * Nazwa modulu    : icmp_lib.c           *
 * Data            : 12-25-03 16:18:00:00 *
 ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/ip_icmp.h>
#include "icmp_libr.h"

int icmp_send(int *sockfd, struct sockaddr_in *remote_address, int *ttl, int *_icmp_id, int *_icmp_seq)
{
    struct icmp icmp_packet;                                        //a teraz tworzymy strukture segmentu ICMP
    icmp_packet.icmp_type = ICMP_TYPE_ECHO_REQ;                     //typ to request (typ: 8)
    icmp_packet.icmp_code = ICMP_CODE_ECHO_REQ;                     //kod to request (kod: 0)
    icmp_packet.icmp_id =  *_icmp_id;
    icmp_packet.icmp_seq = (uint16_t)htons(*_icmp_seq);
    icmp_packet.icmp_cksum = 0;
    icmp_packet.icmp_cksum = in_cksum((uint16_t*)&icmp_packet, 8, 0);

    if(setsockopt(*sockfd, IPPROTO_IP, IP_TTL, ttl, sizeof(int)))
        return -1;

    if(sendto(*sockfd, &icmp_packet, 8, 0, (struct sockaddr*)remote_address, sizeof(*remote_address)) < 0)
        return -1;

    return 0 ;
}

struct reply icmp_recv(int *sockfd, int *_icmp_id, struct timeval *start, int *_icmp_seq) {
    struct timeval end;
    struct reply this_reply;
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    buffer_ptr = buffer;
    this_reply.repl_time = -1;
    if(recvfrom(*sockfd, buffer_ptr, 65536, 0, (struct sockaddr*)&sender, &sender_len) < 0)
        return this_reply;

    inet_ntop(AF_INET, &sender.sin_addr, this_reply.ip_address, sizeof(this_reply.ip_address));
    struct ip* packet = (struct ip*) buffer_ptr;
    buffer_ptr += ((packet->ip_hl)*4);

    struct icmp* icmp_packet = (struct icmp*) buffer_ptr;
    buffer_ptr += 8;

    if((icmp_packet->icmp_type == ICMP_TYPE_TIME_EXCEEDED) && (icmp_packet->icmp_code == ICMP_CODE_EXC_TTL)){
        struct ip* packet_orig = (struct ip*) buffer_ptr;
        buffer_ptr += ((packet_orig->ip_hl)*4);
        struct icmp* icmp_orig = (struct icmp*) buffer_ptr;
        if(((icmp_orig->icmp_id) != *_icmp_id) || (htons(icmp_orig->icmp_seq) != *_icmp_seq))
            return this_reply;
        } else if((icmp_packet->icmp_id != *_icmp_id) || (htons(icmp_packet->icmp_seq) != *_icmp_seq))
            return this_reply;
    gettimeofday(&end, NULL);
    this_reply.repl_time = (double)(1000000*(end.tv_sec - start->tv_sec) + (end.tv_usec - start->tv_usec))/1000;

    return this_reply;
}


uint16_t in_cksum(const uint16_t *addr, register int len, uint16_t csum)
{
    register int nleft = len;
    const uint16_t *w = addr;
    register uint16_t answer;
    register int sum = csum;

    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
        sum += htons(*(u_char *)w << 8);

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}


