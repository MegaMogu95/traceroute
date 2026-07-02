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

# define ICMP_HDRLEN  8
# define ICMP_PKTLEN  64


//utils
int		ft_strcmp(const char *s1, const char *s2);
void	ft_memset(void *s, int c, size_t n);
void	ft_memcpy(void *dest, const void *src, size_t n);

//network
void    resolve_host(const char *host, struct sockaddr_in *addr, 
										char *ip, size_t ip_size);
int		create_socket();
void	send_icmp_packet(int sockfd, const struct sockaddr_in *addr, uint16_t  seq, uint8_t ttl);

void	run_route(struct sockaddr_in addr, char *ip, int sockfd);

#endif