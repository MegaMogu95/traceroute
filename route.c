#include "traceroute.h"

static uint16_t	ttl = FIRST_TTL;
static uint16_t	dport = FIRST_DPORT;

static void	send_batch(int sockfd, struct sockaddr_in *addr)
{
	int		sent;
	char	payload[UDP_DATALEN];

	ft_memset(payload, 0, UDP_DATALEN);
	sent = 0;
	while (sent < SQUERIES && ttl < MAX_TTL)
	{
		addr->sin_port = htons(dport);
		sendto(sockfd, payload, UDP_DATALEN, 0, (const struct sockaddr *)addr, sizeof(*addr));
		sent++;
		dport++;
		if (sent % NQUERIES == 0)
		{
			ttl++;
			if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)))
			{
				fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
				close(sockfd);
				exit(1);
			}
		}
	}
}

void	route(int sockfd, struct sockaddr_in *addr)
{
	send_batch(sockfd, addr);
}