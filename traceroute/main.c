/******************************************
 * Imie i nazwisko : Marek Jenda          *
 * Nr indeksu      : 241647               *
 * Nazwa programu  : traceroute           *
 * Nazwa modulu    : main.c               *
 * Data            : 12-25-03 16:18:00:00 *
 ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include "icmp_libr.h"
#include <unistd.h>

void compare_and_print(char *ip1, char *ip2, char *ip3, int i);

int main(int argc, char** argv)
{
    argc = argc;
    fd_set sockfd_select;                                                   //suma czasu odbioru dla danego ttl
    struct timeval start, tv;                                               //poczatek odliczania, czas dla select
    struct reply replies[3];                                                //trzy odpowiedzi
    struct sockaddr_in remote_address;                                      //tworzymy strukture adresu internetowego
    double timer;
    int ttl, i, sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);           //deskryptor gniazda

    if(sockfd < -1) {
        printf("ERR: Descriptor completion unsuccessful.\n");
        return -1;
    };

    gettimeofday(&start, NULL);
    srand(start.tv_usec);
    int id = htons(rand());

    bzero(&remote_address, sizeof(remote_address));                         //zerujemy wszystkie pola
    remote_address.sin_family = AF_INET;                                    //bedzie to adres IPv4
    if(inet_pton(AF_INET, argv[1], &remote_address.sin_addr) != 1) {        //adres IPv4
        printf("ERR: Input format - ./traceroute DOTTED_IPv4_ADDR\n");
        return -1;
    }

    for(ttl = 1; ttl <= 30; ttl++) {                                        //uruchamiamy 30 iteracji zapytan
        timer = 0;                                                          //czas odbioru wstepnie jest rowny 0
        gettimeofday(&start, NULL);                                         //zaczynamy mierzyc czas

        for(i = 0; i < 3; i++)                                              //wysylamy trzy zapytania ICMP_ECHO_REQ
            if(icmp_send(&sockfd, &remote_address, &ttl, &id, &ttl) < 0) {
                printf("ERR: Unable to set options on socket or couldn't sent packet. R U Root?\n");
                return -1;
            }

        FD_ZERO(&sockfd_select);                                            //czysci zestaw deskryptorow dla select()
        FD_SET(sockfd, &sockfd_select);                                     //dodaje deskryptor sockfd do zestawu
        tv.tv_sec = 1;                                                      //na odp bedziemy czekac 1s
        tv.tv_usec = 0;

        for(i = 0; i < 3 && select(sockfd + 1, &sockfd_select, 0, 0, &tv); i++) { //odbieramy pakiety
            replies[i] = icmp_recv(&sockfd, &id, &start, &ttl);                   //cos jest w deksryptorze
            if(replies[i].repl_time == -1) {                                      //to nie jest nasz pakiet
                i--;
                continue;                                                         //a wiec czekamy dalej
            }
            timer += replies[i].repl_time;                                        //dodajemy czas odbioru
        }

        printf("%3d.", ttl);
        if(i != 0) {
            compare_and_print(replies[0].ip_address, replies[0].ip_address, replies[0].ip_address, i);
            if (i == 3) printf("%3.0fms\n", timer/i);
            else printf("??? \n");
        } else {
            printf(" *\n"); continue; }

       if(!strcmp(replies[0].ip_address, argv[1]) || !strcmp(replies[1].ip_address, argv[1]) || !strcmp(replies[2].ip_address, argv[1])) break;
     }
    close(sockfd);
    return 0;
}

void compare_and_print(char *ip1, char *ip2, char *ip3, int i){
    printf("%15s ", ip1);
    if(strcmp(ip1, ip2) && i > 1) printf("%15s ", ip2);
    else if(strcmp(ip1, ip3) && i > 2) printf("%15s ", ip3);
    if(strcmp(ip1, ip3) && strcmp(ip2, ip3) && i > 2) printf("%15s ", ip3);
}
