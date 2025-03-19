#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define MCAST_GROUP "ff02::1"  // Multicast address (IPv6)
#define MCAST_PORT 12345       // Multicast port
#define TTL 1                  // Time-to-live for multicast packets (0-255)
#define INTERVAL 1             // Interval in seconds between packets

int main() {
    struct sockaddr_in6 group_addr;
    int sock;
    const char *message = "Hello from IPv6 multicast server!";
    int loop = 1;  // Enable multicast loopback
    int ttl = TTL;

    // Create an IPv6 UDP socket
    sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket option to enable multicast loopback
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
        perror("setsockopt IPV6_MULTICAST_LOOP");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Set socket option for multicast TTL
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IPV6_MULTICAST_HOPS");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Set up the multicast group address
    memset(&group_addr, 0, sizeof(group_addr));
    group_addr.sin6_family = AF_INET6;
    group_addr.sin6_port = htons(MCAST_PORT);
    if (inet_pton(AF_INET6, MCAST_GROUP, &group_addr.sin6_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Starting IPv6 multicast server, sending to %s:%d\n", MCAST_GROUP, MCAST_PORT);

    while (1) {
        // Send the multicast packet
        ssize_t sent_bytes = sendto(sock, message, strlen(message), 0,
                                    (struct sockaddr *)&group_addr, sizeof(group_addr));
        if (sent_bytes < 0) {
            perror("sendto");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Sent multicast message: %s\n", message);

        // Wait for the next interval
        sleep(INTERVAL);
    }

    close(sock);
    return 0;
}
