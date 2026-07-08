#include "traceroute.h"

void	print_help(void)
{
	printf("Usage:\n");
	printf("\tft_traceroute [options] host [packet_len]\n");
	printf("\thost:\tThe host to traceroute to\n");
	printf("Options:\n");
	printf("\t-h, --help\t\tPrint this help and exit\n");
	printf("\t-m max_ttl\t\tMaximum number of hops (default %d)\n", MAX_TTL);
	printf("\t-q nqueries\t\tProbes sent per hop (default %d)\n", NQUERIES);
	printf("\t-N squeries\t\tProbes sent simultaneously (default %d)\n", SQUERIES);
	printf("\t-n\t\t\tDo not resolve addresses to hostnames\n");
	printf("\t--packet_len len\tTotal probe size in bytes (default %d)\n",
		IP_HDRLEN + UDP_PKTLEN);
}

int	main(int argc, char **argv)
{
	struct sockaddr_in	addr;
	char				ip[INET_ADDRSTRLEN];
	int					send_sockfd;
	int					recv_sockfd;

	parse_args(argc, argv);
	resolve_host(g_cfg.host, &addr, ip, INET_ADDRSTRLEN);
	send_sockfd = create_send_socket();
	recv_sockfd = create_recv_socket();
	printf("traceroute to %s (%s), %d hops max, %d byte packets\n",
		g_cfg.host, ip, g_cfg.max_ttl, g_cfg.packet_len);
	route(send_sockfd, recv_sockfd, &addr);
}