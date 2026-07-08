#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <sys/time.h> //gettimeofday
# include <sys/types.h> //timeval
# include <unistd.h> //getpid
# include <stdio.h> //printf, fprintf
# include <sys/socket.h> //socket recv
# include <errno.h> //errno
# include <string.h> //strerror
# include <stdlib.h> //exit
# include <netinet/in.h> //IPPROTO_ICMP, INTE_ADDRSTRLEN
# include <netinet/ip_icmp.h> //ICMP_ECHO
# include <arpa/inet.h> //inet_ntoa
# include <stddef.h> //size_t
# include <stdint.h>
# include <netdb.h>
# include <linux/errqueue.h> //sock_extended_err, SO_EE_OFFENDER
# include <limits.h> //INT_MAX

# define IP_HDRLEN 20
# define UDP_HDRLEN  8
# define UDP_DATALEN 32
# define UDP_PKTLEN 40

# define TIMEOUT 1

// defaults; also serve as the upper bounds accepted by the options
# define NQUERIES 3
# define SQUERIES 16

# define FIRST_DPORT 33434

# define FIRST_TTL 1
# define MAX_TTL 30

// hard limits for the corresponding options
# define MAX_HOPS 255
# define MAX_PACKET_LEN 65000
# define MIN_PACKET_LEN (IP_HDRLEN + UDP_HDRLEN)

//route
typedef struct s_result
{
	char			received; //wether an ICMP answer associated with the dport was received
	struct in_addr	from; //who sent the answer
	char			reached; //wether an ICMP host unreachable was received
	uint8_t			code; //ICMP host unreachable code
	uint16_t		ttl;
	uint16_t		query;
	struct timeval	tv_start;
	struct timeval	tv_end; //received timestamp
}	t_result;

//parsing
typedef struct s_config
{
	int		max_ttl;	// -m : maximum number of hops probed
	int		nqueries;	// -q : probes sent per hop
	int		squeries;	// -N : probes sent simultaneously per batch
	int		packet_len;	// total probe size (IP + UDP header + payload) in bytes
	char	*host;		// destination operand
}	t_config;

extern t_config	g_cfg;

void	print_help(void);
void	parse_args(int argc, char **argv);

//utils
int		ft_strcmp(const char *s1, const char *s2);
void	ft_memset(void *s, int c, size_t n);
void	ft_memcpy(void *dest, const void *src, size_t n);

//network
void    resolve_host(const char *host, struct sockaddr_in *addr, 
										char *ip, size_t ip_size);
int		create_send_socket();
int		create_recv_socket();
void	send_udp_packet(int sockfd, const struct sockaddr_in *addr, uint16_t  seq, uint8_t ttl);

void	route(int send_sockfd, int recv_sockfd, struct sockaddr_in *addr);
void	receive_batch(int sockfd);
void	report(void);

#endif