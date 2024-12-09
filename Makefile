NAME = minishell
SRCS = $(shell find source -type f -name "*.c")
OBJS = $(SRCS:.c=.o)
LIBFT = includes/libft/libft.a
INLCLUDES = includes
CFLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	cc $(CFLAGS) $(OBJS) -I $(INLCLUDES) $(LIBFT) -o $(NAME) -lreadline
	
$(LIBFT):
	make -C includes/libft

%.o: %.c
	cc $(CFLAGS) -c $< -o $@

clean:
	$(shell find . -type f -name "*.o" -delete)
	make -C includes/libft clean

fclean: clean
	$(shell find . -type f -name "$(NAME)" -delete)
	make -C includes/libft fclean

re: fclean all

.PHONY: all clean fclean re