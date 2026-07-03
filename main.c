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
	int					sockfd;

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
	sockfd = create_socket();
	route(sockfd, &addr);
}