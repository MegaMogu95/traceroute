#include "traceroute.h"

static uint16_t			ttl = FIRST_TTL;
static uint16_t			dport = FIRST_DPORT;

// results for the probes sent in the current batch, indexed by their
// position within the batch [0, batch_count)
static t_result			results[SQUERIES];
static uint16_t			batch_start_dport;

static void	send_batch(int sockfd, struct sockaddr_in *addr)
{
	int		sent;
	int		idx;
	char	payload[UDP_DATALEN] = {0};

	batch_start_dport = dport;
	sent = 0;
	while (sent < SQUERIES && ttl < MAX_TTL)
	{
		idx = dport - FIRST_DPORT;
		results[sent].received = 0;
		results[sent].ttl = FIRST_TTL + idx / NQUERIES;
		results[sent].query = idx % NQUERIES;

		addr->sin_port = htons(dport);
		sendto(sockfd, payload, UDP_DATALEN, 0, (const struct sockaddr *)addr, sizeof(*addr));
		gettimeofday(&results[sent].tv_start, NULL);
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

//return 0 when ip reached or ttl > MAX_TTL
int	report_batch(void)
{
	char		ip[INET_ADDRSTRLEN];
	t_result	*r;
	int			i;
	double		rtt;
	char		reached;
	const char	*annotation[16] = {
		"!N",
		"!H",
		"!P",
		"",
		"!F",
		"!S",
		"!6",
		"!7",
		"!8",
		"!9",
		"!10",
		"!11",
		"!12",
		"!X",
		"!V",
		"!C"
	};

	reached = 0;
	for (i = 0; i < SQUERIES; i++)
	{
		r = &results[i];
		if (r->ttl > MAX_TTL)
			return (0);
		if (r->query == 0)
			printf("%2u ", r->ttl);
		if (!r->received)
			printf(" *");
		else
		{
			inet_ntop(AF_INET, &r->from, ip, sizeof(ip));
			rtt = (r->tv_end.tv_sec - r->tv_start.tv_sec) * 1000 + (r->tv_end.tv_usec - r->tv_start.tv_usec) / 1000;
			printf("  (%s)  %.3f ms", ip, rtt);
			if (r->reached)
			{
				printf(" %s", annotation[r->code]);
				reached = 1;
			}
		}
		if (r->query == NQUERIES - 1)
		{
			printf("\n");
			if (reached)
				return (0);
		}
	}
	return (1);
}

void	route(int sockfd, struct sockaddr_in *addr)
{
	send_batch(sockfd, addr);
	results[0].received = 1;
	results[1].received = 1;
	results[2].received = 1;
	results[0].from = *(struct in_addr *)addr;
	results[1].from = *(struct in_addr *)addr;
	results[2].from = *(struct in_addr *)addr;
	// receive_batch(sockfd);
	report_batch();
	send_batch(sockfd, addr);
	results[4].received = 1;
	results[4].reached = 1;
	report_batch();
}