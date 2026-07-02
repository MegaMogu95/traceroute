NAME    = ft_traceroute

CC      = cc
CFLAGS  = -Wall -Wextra -Werror

SRCS    = main.c network.c utils.c
OBJS    = $(SRCS:.c=.o)

HEADER  = traceroute.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
