#include "traceroute.h"

void    resolve_host(const char *host, struct sockaddr_in *addr, 
										char *ip, size_t ip_size)
{
    struct addrinfo     hints;
    struct addrinfo     *res;
    struct sockaddr_in  *sin;
	int					err;

    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

	err = getaddrinfo(host, NULL, &hints, &res);
    if (err || res == NULL)
    {
        fprintf(stderr, "ft_ping: getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    sin = (struct sockaddr_in *)res->ai_addr;
    if (addr != NULL)
        ft_memcpy(addr, sin, sizeof(struct sockaddr_in));
    if (ip != NULL && ip_size > 0)
        inet_ntop(AF_INET, &sin->sin_addr, ip, ip_size);

    freeaddrinfo(res);
}

int	create_socket()
{
	int				sockfd;
	struct timeval	timeout;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		fprintf(stderr, "ft_traceroute: socket: %s\n", strerror(errno));
		exit(1);
	}

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }

	return (sockfd);
}

static uint16_t in_checksum(const void *data, size_t len)
{
        const uint16_t	*p;
        uint32_t		sum;

        p = data;
        sum = 0;
        while (len > 1)
        {
                sum += *p++;
                len -= 2;
        }
        if (len)
                sum += *(const uint8_t *)p;
        while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);
        return ((uint16_t)~sum);
}

static void	build_icmp_packet(char packet[ICMP_PKTLEN], uint16_t seq)
{
	struct icmphdr	*icmp;
	struct timeval	*timestamp;

	ft_memset(packet, 0, ICMP_PKTLEN);

	icmp 					= (struct icmphdr *)packet;
	icmp->type				= ICMP_ECHO;
	icmp->code				= 0;
	icmp->un.echo.id		= htons(getpid() & 0xFFFF);
	icmp->un.echo.sequence	= htons(seq);

	timestamp = (struct timeval *)(packet + ICMP_HDRLEN);
	gettimeofday(timestamp, NULL);

	icmp->checksum	= in_checksum(packet, ICMP_PKTLEN);
}

void	send_icmp_packet(int sockfd, const struct sockaddr_in *addr, uint16_t  seq, uint8_t ttl)
{
	char	packet[ICMP_PKTLEN];

	build_icmp_packet(packet, seq);
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
    {
        fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }
	sendto(sockfd, packet, ICMP_PKTLEN, 0, (const struct sockaddr *)addr, sizeof(*addr));
}