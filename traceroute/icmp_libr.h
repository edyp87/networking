/******************************************
 * Imie i nazwisko : Marek Jenda          *
 * Nr indeksu      : 241647               *
 * Nazwa programu  : traceroute           *
 * Nazwa modulu    : icmp_lib.h           *
 * Data            : 12-25-03 16:18:00:00 *
 ******************************************/

#define ICMP_TYPE_ECHO_REQ      8
#define   ICMP_CODE_ECHO_REQ    0
#define ICMP_TYPE_TIME_EXCEEDED 11
#define   ICMP_CODE_EXC_TTL     0

struct reply {
    char          ip_address[20];
    double        repl_time;
};

unsigned char buffer[65536];
unsigned char *buffer_ptr;
int           remaining_packet_data;

int icmp_send(int *socketfd, struct sockaddr_in *remote_address, int *ttl, int *_icmp_id, int *_icmp_seq);
struct reply icmp_recv(int *sockfd, int *_icmp_id, struct timeval *start, int *_icmp_seq);
uint16_t in_cksum(const uint16_t *addr, register int len, uint16_t csum);

