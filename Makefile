CC      := cc
CFLAGS  := -Wall -Wextra -Werror -pthread #-fsanitize=thread
NAME    := codexion
INC		:= struct_elements.h codexion.h
SRC     := actions_threads.c codexion.c init_elements.c parsing.c\
		clear.c coders_thread.c threads_manager.c scheduler.c\
		dongle_thread.c
OBJS    := $(SRC:.c=.o)

.PHONY: all clean fclean re try leak

all: $(NAME)

$(NAME): $(OBJS) $(INC)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

run: re
	@echo "\n\n===========CODEXION==============\n\n"
	@./codexion 1 800 100 100  100 10 50 "fifo"
	@echo "\n\n===========CODEXION==============\n\n"
	@make fclean

# DO NOT EXECUTE THIS IF THE FLAG -FSANITIZE=THREAD
leak: re
	@echo "\n\n===========CODEXION==============\n\n"

	@valgrind  --leak-check=full ./codexion 1 800 100 100  100 10 50 "edf"
	@echo "\n\n===========CODEXION==============\n\n"
	@make fclean

GREEN = \033[0;32m
RED   = \033[0;31m
RESET = \033[0m
tests: re
	@echo "Lancement du stress test (10 itérations)..."
	@i=1; while [ $$i -le 10 ]; do \
		echo -n "Test $$i/10 : "; \
		./codexion 10 800 100 100 100 5 50 "fifo" > test_output.log 2>&1; \
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
	@$(MAKE) fclean


re: fclean all