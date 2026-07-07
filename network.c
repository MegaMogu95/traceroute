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
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

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

int	create_send_socket()
{
	int				sockfd;
	int				ttl;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd == -1)
	{
		fprintf(stderr, "ft_traceroute: socket: %s\n", strerror(errno));
		exit(1);
	}

	ttl = FIRST_TTL;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)))
	{
		fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}

	return (sockfd);
}

int create_recv_socket()
{
	int				sockfd;
	struct timeval	timeout;

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		fprintf(stderr, "ft_traceroute: socket: %s\n", strerror(errno));
		exit(1);
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        fprintf(stderr, "ft_ping: setsockopt IP_TTL: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }

	return (sockfd);
}

// static uint16_t in_checksum(const void *data, size_t len)
// {
//         const uint16_t	*p;
//         uint32_t		sum;

//         p = data;
//         sum = 0;
//         while (len > 1)
//         {
//                 sum += *p++;
//                 len -= 2;
//         }
//         if (len)
//                 sum += *(const uint8_t *)p;
//         while (sum >> 16)
//                 sum = (sum & 0xffff) + (sum >> 16);
//         return ((uint16_t)~sum);
// }