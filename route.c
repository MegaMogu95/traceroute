#include "traceroute.h"

static uint16_t			ttl = FIRST_TTL;
static uint16_t			dport = FIRST_DPORT;
static struct timeval	timestamps[(MAX_TTL - FIRST_TTL + 1) * NQUERIES];
char					seen[(MAX_TTL - FIRST_TTL + 1) * NQUERIES];

// results for the probes sent in the current batch, indexed by their
// position within the batch [0, batch_count)
static t_result			results[SQUERIES];
static uint16_t			batch_start_dport;
static int				batch_count;

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
		results[sent].reached = 0;
		results[sent].ttl = FIRST_TTL + idx / NQUERIES;
		results[sent].query = idx % NQUERIES;

		addr->sin_port = htons(dport);
		sendto(sockfd, payload, UDP_DATALEN, 0, (const struct sockaddr *)addr, sizeof(*addr));
		gettimeofday(&timestamps[idx], NULL);
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
	batch_count = sent;
}

// Drain the socket error queue for the ICMP replies triggered by the
// probes of the current batch. Each reply carries a struct sock_extended_err
// in an IP_RECVERR control message; msg_name holds the probe's original
// destination, whose port tells us which probe (and thus which slot) it is.
void	receive_batch(int sockfd)
{
	char						cbuf[512];
	char						buf[512];
	struct msghdr				msg;
	struct iovec				iov;
	struct sockaddr_in			from;
	struct cmsghdr				*cmsg;
	struct sock_extended_err	*ee;
	struct sockaddr_in			*offender;
	struct timeval				now;
	int							slot;
	int							idx;

	while (1)
	{
		iov = (struct iovec){buf, sizeof(buf)};
		msg = (struct msghdr){0};
		msg.msg_name = &from;
		msg.msg_namelen = sizeof(from);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = cbuf;
		msg.msg_controllen = sizeof(cbuf);

		if (recvmsg(sockfd, &msg, MSG_ERRQUEUE) < 0)
			break;
		gettimeofday(&now, NULL);

		slot = ntohs(from.sin_port) - batch_start_dport;
		idx = ntohs(from.sin_port) - FIRST_DPORT;
		if (slot < 0 || slot >= batch_count)
			continue;

		cmsg = CMSG_FIRSTHDR(&msg);
		while (cmsg != NULL)
		{
			if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_RECVERR)
			{
				ee = (struct sock_extended_err *)CMSG_DATA(cmsg);
				if (ee->ee_origin == SO_EE_ORIGIN_ICMP)
				{
					offender = (struct sockaddr_in *)SO_EE_OFFENDER(ee);
					results[slot].received = 1;
					results[slot].reached = (ee->ee_type == ICMP_DEST_UNREACH
							&& ee->ee_code == ICMP_PORT_UNREACH);
					results[slot].from = offender->sin_addr;
					results[slot].rtt =
						(now.tv_sec - timestamps[idx].tv_sec) * 1000.0
						+ (now.tv_usec - timestamps[idx].tv_usec) / 1000.0;
				}
			}
			cmsg = CMSG_NXTHDR(&msg, cmsg);
		}
	}
}

void	report(void)
{
	char		ip[INET_ADDRSTRLEN];
	t_result	*r;
	int			last_ttl;
	int			i;

	last_ttl = -1;
	for (i = 0; i < batch_count; i++)
	{
		r = &results[i];
		if (r->ttl != last_ttl)
		{
			printf("\n%2u ", r->ttl);
			last_ttl = r->ttl;
		}
		if (!r->received)
			printf(" *");
		else
		{
			inet_ntop(AF_INET, &r->from, ip, sizeof(ip));
			printf("  %s  %.3f ms", ip, r->rtt);
		}
	}
	printf("\n");
}

void	route(int sockfd, struct sockaddr_in *addr)
{
	ft_memset(seen, 0, sizeof(seen));
	send_batch(sockfd, addr);
	receive_batch(sockfd);
	report();
}