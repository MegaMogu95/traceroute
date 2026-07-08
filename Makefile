NAME    = ft_traceroute
BNAME	= ft_traceroute_bonus

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -g

SRCS    = mandatory/main.c mandatory/network.c mandatory/utils.c mandatory/route.c
BSRCS	= bonus/main.c bonus/network.c bonus/utils.c bonus/route.c
OBJS    = $(SRCS:.c=.o)
BOBJS	= $(BSRCS:.c=.o)
INC		= mandatory
BINC	= bonus

all: $(NAME)

bonus: $(BNAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -I$(INC)

$(BNAME): $(BOBJS)
	$(CC) $(CFLAGS) $(BOBJS) -o $(BNAME) -I$(BINC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BOBJS)

fclean: clean
	rm -f $(NAME) $(BNAME)

re: fclean all

.PHONY: all clean fclean re bonus
