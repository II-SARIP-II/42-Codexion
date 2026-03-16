CC      := cc
CFLAGS  := -Wall -Wextra -Werror -pthread
NAME    := codexion

SRC     := actions_threads.c codexion.c init_elements.c parsing.c\
		errors.c coders_thread.c threads_manager.c
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

try: re
	@echo "\n\n===========CODEXION==============\n\n"
	@./codexion 10 800 200 200  200 10 50 1
	@echo "\n\n===========CODEXION==============\n\n"
	@make clean

re: fclean all