NAME = minishell
SRCS = $(wildcard source/*.c)
OBJS = $(SRCS:.c=.o)
LIBFT = includes/libft/libft.a
INLCLUDES = includes
CFLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	cc $(CFLAGS) $(OBJS) -I $(INLCLUDES) $(LIBFT) -o $(NAME)
	
$(LIBFT):
	make -C Libft

%.o: %.c
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	make -C Libft clean

fclean: clean
	rm -f $(NAME)
	make -C Libft fclean

re: fclean all

.PHONY: all clean fclean re