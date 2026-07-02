#include "traceroute.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2 && *s1)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

void	ft_memset(void *s, int c, size_t n)
{
	unsigned char	*p;

	p = s;
	for (size_t i = 0; i < n; i++)
		p[i] = c;
}

void	ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	d = dest;
	s = src;
	for (size_t i = 0; i < n; i++)
		d[i] = s[i];
}