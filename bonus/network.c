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
        fprintf(stderr, "ft_traceroute: getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    sin = (struct sockaddr_in *)res->ai_addr;
    if (addr != NULL)
        ft_memcpy(addr, sin, sizeof(struct sockaddr_in));
    if (ip != NULL && ip_size > 0)
    {
        char    *str;
        size_t  i;

        str = inet_ntoa(sin->sin_addr);
        i = 0;
        while (str[i] && i + 1 < ip_size)
        {
            ip[i] = str[i];
            i++;
        }
        ip[i] = '\0';
    }

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
		fprintf(stderr, "ft_traceroute: setsockopt IP_TTL: %s\n", strerror(errno));
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

	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        fprintf(stderr, "ft_traceroute: setsockopt IP_TTL: %s\n", strerror(errno));
        close(sockfd);
        exit(1);
    }

	return (sockfd);
}