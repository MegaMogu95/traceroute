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
		if (idx % NQUERIES == NQUERIES - 1)
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


// Drain the raw ICMP socket, correlating each ICMP Time Exceeded / Destination
// Unreachable to the probe that triggered it via the destination port echoed
// back in the quoted UDP header. Loops until recv returns -1 (SO_RCVTIMEO
// timeout). Received probes get received/from/reached/code/tv_end set; probes
// with no matching reply are left untouched.
void	receive_batch(int sockfd)
{
	uint8_t				buf[512];
	struct sockaddr_in	from;
	socklen_t			fromlen;
	ssize_t				n;
	size_t				ip_hlen;
	size_t				inner;
	size_t				inner_hlen;
	uint8_t				type;
	uint16_t			dport;
	int					idx;
	t_result			*r;

	while (1)
	{
		fromlen = sizeof(from);
		n = recvfrom(sockfd, buf, sizeof(buf), 0,
				(struct sockaddr *)&from, &fromlen);
		if (n < 0)
			break ;
		ip_hlen = (buf[0] & 0x0f) * 4;
		if ((size_t)n < ip_hlen + 8)
			continue ;
		type = buf[ip_hlen];
		if (type != ICMP_TIME_EXCEEDED && type != ICMP_DEST_UNREACH)
			continue ;
		// quoted original datagram: IP header + at least the 8-byte UDP header
		inner = ip_hlen + 8;
		if ((size_t)n < inner + 20)
			continue ;
		inner_hlen = (buf[inner] & 0x0f) * 4;
		if (buf[inner + 9] != IPPROTO_UDP)
			continue ;
		if ((size_t)n < inner + inner_hlen + 8)
			continue ;
		dport = ((uint16_t)buf[inner + inner_hlen + 2] << 8)
			| buf[inner + inner_hlen + 3];
		idx = (int)dport - (int)batch_start_dport;
		if (idx < 0 || idx >= SQUERIES || results[idx].received)
			continue ;
		r = &results[idx];
		r->received = 1;
		r->from = from.sin_addr;
		r->reached = (type == ICMP_DEST_UNREACH);
		r->code = buf[ip_hlen + 1];
		gettimeofday(&r->tv_end, NULL);
	}
}

//return 0 when ip reached or ttl > MAX_TTL
int	report_batch()
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
			rtt = (double)(r->tv_end.tv_sec - r->tv_start.tv_sec) * 1000 + (double)(r->tv_end.tv_usec - r->tv_start.tv_usec) / 1000;
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

void	route(int send_sockfd, int recv_sockfd, struct sockaddr_in *addr)
{
	do
	{
		send_batch(send_sockfd, addr);
		receive_batch(recv_sockfd);
	} while (report_batch());
}