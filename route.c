#include "traceroute.h"



static void	run_ttl(struct sockaddr_in *addr, char *ip, int sockfd, 
											uint16_t *seq, uint8_t ttl)
{
	char		buf[1024];
	ssize_t		len;
	uint16_t	i;

	i = 0;
	while (i < 3)
	{
		send_icmp_packet(sockfd, addr, *seq, ttl);
		len = recv(sockfd, buf, sizeof(buf), 0);
		if (len > 0)
		{

		}
}

void	run_route(struct sockaddr_in *addr, char *ip, int sockfd)
{
	for (int i = 0; i < 3; i++)
	{
		send_icmp_packet(sockfd, addr, seq, ttl);
	}
}