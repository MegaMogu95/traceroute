#include "traceroute.h"

void	print_help()
{
	printf("Usage:\n");
	printf("\tft_traceroute host\n");
	printf("\thost:\tThe host to traceroute to\n");
	printf("Options:\n");
	printf("\t-h\t--help\n");
}

int	main(int argc, char **argv)
{
	struct sockaddr_in	addr;
	char				ip[INET_ADDRSTRLEN];
	int					send_sockfd;
	int					recv_sockfd;

	if (argc != 2)
	{
		fprintf(stderr, "error: wrong number of arguments\n");
		return (1);
	}

	if (!ft_strcmp(argv[1], "-h") || !ft_strcmp(argv[1], "--help"))
	{
		print_help();
		return (0);
	}

	resolve_host(argv[1], &addr, ip, INET_ADDRSTRLEN);
	send_sockfd = create_send_socket();
	recv_sockfd = create_recv_socket();
	printf("traceroute to %s (%s), %d hops max, %d byte packets\n", argv[1], ip, MAX_TTL, IP_HDRLEN + UDP_PKTLEN);
	route(send_sockfd, recv_sockfd, &addr);
}