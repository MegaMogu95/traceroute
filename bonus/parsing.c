#include "traceroute.h"

t_config	g_cfg = {
	.max_ttl = MAX_TTL,
	.nqueries = NQUERIES,
	.squeries = SQUERIES,
	.packet_len = IP_HDRLEN + UDP_PKTLEN,
	.numeric = 0,
	.host = NULL,
};

// Parse a strictly non-negative integer option value, aborting with a
// traceroute-style error when the value is empty, non-numeric or overflows.
static int	parse_value(const char *s, const char *name)
{
	long	val;
	int		i;

	if (s[0] == '\0')
	{
		fprintf(stderr, "ft_traceroute: %s: missing value\n", name);
		exit(1);
	}
	val = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9' || val > INT_MAX)
		{
			fprintf(stderr, "ft_traceroute: %s: invalid value (`%s')\n", name, s);
			exit(1);
		}
		val = val * 10 + (s[i] - '0');
		i++;
	}
	return ((int)val);
}

static void	set_packet_len(const char *value)
{
	g_cfg.packet_len = parse_value(value, "packet_len");
}

static void	set_option(char opt, const char *value)
{
	if (opt == 'm')
		g_cfg.max_ttl = parse_value(value, "max hops");
	else if (opt == 'q')
		g_cfg.nqueries = parse_value(value, "nqueries");
	else if (opt == 'N')
		g_cfg.squeries = parse_value(value, "squeries");
}

// Handle one dashed argument (argv[i]). Supports "-m 5" / "-m5" for the valued
// short options and the long "--packet_len" form. Returns how many argv
// entries were consumed (1 or 2).
static int	parse_option(int argc, char **argv, int i)
{
	char	opt;

	if (!ft_strcmp(argv[i], "--packet_len"))
	{
		if (i + 1 >= argc)
		{
			fprintf(stderr, "ft_traceroute: option `--packet_len' requires an argument\n");
			exit(1);
		}
		set_packet_len(argv[i + 1]);
		return (2);
	}
	if (!ft_strcmp(argv[i], "-n"))
	{
		g_cfg.numeric = 1;
		return (1);
	}
	opt = argv[i][1];
	if (opt != 'm' && opt != 'q' && opt != 'N')
	{
		fprintf(stderr, "ft_traceroute: invalid option -- `%s'\n", argv[i]);
		exit(1);
	}
	if (argv[i][2] != '\0')
	{
		set_option(opt, argv[i] + 2);
		return (1);
	}
	if (i + 1 >= argc)
	{
		fprintf(stderr, "ft_traceroute: option requires an argument -- `%c'\n", opt);
		exit(1);
	}
	set_option(opt, argv[i + 1]);
	return (2);
}

// Clamp/reject option values against their hard limits so the fixed-size
// batch buffers stay valid.
static void	validate(void)
{
	if (g_cfg.host == NULL)
	{
		fprintf(stderr, "ft_traceroute: missing host operand\n");
		exit(1);
	}
	if (g_cfg.max_ttl < 1 || g_cfg.max_ttl > MAX_HOPS)
	{
		fprintf(stderr, "ft_traceroute: max hops must be within 1..%d\n", MAX_HOPS);
		exit(1);
	}
	if (g_cfg.nqueries < 1 || g_cfg.nqueries > SQUERIES)
	{
		fprintf(stderr, "ft_traceroute: nqueries must be within 1..%d\n", SQUERIES);
		exit(1);
	}
	if (g_cfg.squeries < 1 || g_cfg.squeries > SQUERIES)
	{
		fprintf(stderr, "ft_traceroute: squeries must be within 1..%d\n", SQUERIES);
		exit(1);
	}
	if (g_cfg.packet_len < MIN_PACKET_LEN || g_cfg.packet_len > MAX_PACKET_LEN)
	{
		fprintf(stderr, "ft_traceroute: packet_len must be within %d..%d\n",
			MIN_PACKET_LEN, MAX_PACKET_LEN);
		exit(1);
	}
}

void	parse_args(int argc, char **argv)
{
	int	i;
	int	positional;

	positional = 0;
	i = 1;
	while (i < argc)
	{
		if (!ft_strcmp(argv[i], "-h") || !ft_strcmp(argv[i], "--help"))
		{
			print_help();
			exit(0);
		}
		if (argv[i][0] == '-' && argv[i][1] != '\0')
			i += parse_option(argc, argv, i);
		else
		{
			if (positional == 0)
				g_cfg.host = argv[i];
			else if (positional == 1)
				set_packet_len(argv[i]);
			else
			{
				fprintf(stderr, "ft_traceroute: too many arguments\n");
				exit(1);
			}
			positional++;
			i++;
		}
	}
	validate();
}
