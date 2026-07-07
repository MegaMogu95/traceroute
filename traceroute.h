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
# include <arpa/inet.h> //inet_ntop
# include <stddef.h> //size_t
# include <stdint.h>
# include <netdb.h>
# include <linux/errqueue.h> //sock_extended_err, SO_EE_OFFENDER

# define UDP_HDRLEN  8
# define UDP_DATALEN 32
# define UDP_PKTLEN 40

# define TIMEOUT 5

# define NQUERIES 3
# define SQUERIES 16

# define FIRST_DPORT 33433

# define FIRST_TTL 1
# define MAX_TTL 30

//route
typedef struct s_result
{
	char			received;
	struct in_addr	from;
	uint8_t			code;
	uint16_t		ttl;
	uint16_t		query;
	double			rtt;
}	t_result;

//utils
int		ft_strcmp(const char *s1, const char *s2);
void	ft_memset(void *s, int c, size_t n);
void	ft_memcpy(void *dest, const void *src, size_t n);

//network
void    resolve_host(const char *host, struct sockaddr_in *addr, 
										char *ip, size_t ip_size);
int		create_socket();
void	send_udp_packet(int sockfd, const struct sockaddr_in *addr, uint16_t  seq, uint8_t ttl);

void	route(int sockfd, struct sockaddr_in *addr);
void	receive_batch(int sockfd);
void	report(void);

#endif