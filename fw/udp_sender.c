/*---------------------------------------------------------------------------
 * Cedric Adjih - Inria - 2018
 *---------------------------------------------------------------------------*/

#include <unistd.h>
#include <stdio.h>

#include "xtimer.h"
#include "thread.h"

#ifdef USE_POSIX_SOCKET

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#else /* USE_POSIX_SOCKET */

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"

#endif /* USE_POSIX_SOCKET */


#define ENABLE_DEBUG (1)
#include "debug.h"

/*---------------------------------------------------------------------------*/

//#define DST_ADDRESS "2001:660:3207:102::3" /* of demo-fit.saclay.inria.fr */
//#define DST_ADDRESS "fe80::48b3:69ff:febe:8cd3" /* of native@tap1 */
#define DST_ADDRESS "ff02::1"
#define UDP_PORT 0xF0B1 /* = 61617 */

#define PACKET_MAXSIZE 512
static uint8_t _packet_data[PACKET_MAXSIZE];

static const char *_dst_addr_str = DST_ADDRESS;
static int _sleep_delay_sec = 1;

#ifdef USE_POSIX_SOCKET

static void* thread_udp_sender(void *unused)
{
    (void) unused;
    /* -- Cree une socket IPv6 pour UDP */
    int sd = -1;
    sd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sd < 0) {
        printf("Error: cannot create socket.\n");
        return NULL;
    }

    /* -- Adresse IPv6 de destination */
    struct sockaddr_in6 dst_addr;
    dst_addr.sin6_family = AF_INET6;
    /* Convertit une adresse en chaine ASCII vers des octets dans .sin6_addr */
    if (inet_pton(AF_INET6, DST_ADDRESS, &dst_addr.sin6_addr) != 1) {
        printf("Error in destination address: %s\n.", DST_ADDRESS);
        return NULL;
    }
    /* Ecrit le port comme entier 2 octets au format 'reseau' */
    dst_addr.sin6_port = htons(UDP_PORT);

    /* -- Remplissage du paquet avec des octets 'a' */
    memset(_packet_data, 'a', PACKET_MAXSIZE);

    /* -- Boucle principale d'envoi de paquets */
    while (1) {
        //printf("sending UDP packet\n");
        strcpy((char*)_packet_data, "<packet content>");
        int data_size = strlen((char*)_packet_data);
        int status = sendto(sd, _packet_data, data_size, 0,
                            (struct sockaddr *)&dst_addr, sizeof(dst_addr));
        if (status < 0) {
            printf("Erreur dans sendto.\n");
            return NULL;
        }
        xtimer_sleep(_sleep_delay_sec); /* sleep delay */
    }
}

#else /* USE_POSIX_SOCKET */

static void *thread_udp_sender(void *unused)
{
    (void) unused;

    sock_udp_ep_t sock_ep = {
        .family = AF_INET6,
        .port   = UDP_PORT
    };

    if (ipv6_addr_from_str((ipv6_addr_t *)&sock_ep.addr,
                           _dst_addr_str) == NULL) {
        printf("Error while parsing address: %s\n", _dst_addr_str);
        return NULL;
    }
    
    /* -- Remplissage du paquet avec des octets 'a' */
    memset(_packet_data, 'a', PACKET_MAXSIZE);

    /* -- Boucle principale d'envoi de paquets */
    while (1) {
        //printf("sending UDP packet\n");
        strcpy((char*)_packet_data, "<packet content>");
        int data_size = strlen((char*)_packet_data);
        int status = sock_udp_send(NULL, _packet_data, data_size, &sock_ep);
        if (status < 0) {
            printf("error in sendto.\n");
            return NULL;
        }
        xtimer_sleep(_sleep_delay_sec); /* sleep delay */
    }
}

#endif /* USE_POSIX_SOCKET */

/*---------------------------------------------------------------------------*/

static kernel_pid_t udp_sender_pid = KERNEL_PID_UNDEF;
char sender_stack[THREAD_STACKSIZE_MAIN+DEBUG_EXTRA_STACKSIZE];

void start_udp_sender_thread(const char *dst_addr_str, int sleep_delay_sec)
{
    (void) dst_addr_str;
    _dst_addr_str = dst_addr_str;
    (void) sleep_delay_sec;
    _sleep_delay_sec = sleep_delay_sec;
    udp_sender_pid = thread_create(
	sender_stack, sizeof(sender_stack),
	THREAD_PRIORITY_MAIN-1, 0,
	thread_udp_sender, NULL, "udp_sender_thread");
    DEBUG("udp_sender_thread: pid=%d\n", udp_sender_pid);
}

/*---------------------------------------------------------------------------*/
