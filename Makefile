CC      := cc
CFLAGS  := -Wall -Wextra -Werror -pthread # -fsanitize=thread
NAME    := codexion

SRC     := actions_threads.c codexion.c init_elements.c parsing.c\
		errors.c coders_thread.c threads_manager.c
OBJS    := $(SRC:.c=.o)

.PHONY: all clean fclean re try leak

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

run: re
	@echo "\n\n===========CODEXION==============\n\n"
	@./codexion 10 800 100 100  100 10 50 "fifo"
	@echo "\n\n===========CODEXION==============\n\n"
	@make clean

leak: re
	@echo "\n\n===========CODEXION==============\n\n"
	@valgrind  --leak-check=full ./codexion 5 800 100 100  100 10 50 "fifo"
	@echo "\n\n===========CODEXION==============\n\n"
	@make clean

GREEN = \033[0;32m
RED   = \033[0;31m
RESET = \033[0m
tests: re
	@echo "Lancement du stress test (10 itérations)..."
	@i=1; while [ $$i -le 100 ]; do \
		echo -n "Test $$i/100 : "; \
		./codexion 10 800 100 100 100 10 50 "fifo" > test_output.log 2>&1; \
		if grep -q "burned out" test_output.log; then \
			echo "$(RED)[FAILED]$(RESET)"; \
			cat test_output.log; \
			rm -f test_output.log; \
			exit 1; \
		else \
			echo "$(GREEN)[OK]$(RESET)"; \
		fi; \
		i=$$((i+1)); \
	done
	@echo "$(GREEN)Félicitations ! Tous les tests ont réussi.$(RESET)"
	@rm -f test_output.log
	@$(MAKE) clean


re: fclean all