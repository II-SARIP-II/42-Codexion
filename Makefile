CC      := cc
CFLAGS  := -Wall -Wextra -Werror -pthread
NAME    := codexion

SRC     := actions_threads.c codexion.c init_elements.c parsing.c errors.c coders_thread.c
OBJS    := $(SRC:.c=.o)

.PHONY: all clean fclean re try

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

try: all
	./codexion 3 3 3 3  3 3 3 3

re: fclean all