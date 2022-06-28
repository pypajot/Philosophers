SRCDIR := srcs
OBJDIR := objs

SRCS_ := main.c
SRCS := $(patsubst %,$(SRCDIR)/%,$(SRCS_))

OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS_))

NAME := philo

INC := philo.h

CC := gcc
FLAGS := -Wall -Wextra -g -pthread

all : $(NAME)


$(OBJDIR)/%.o : $(SRCDIR)/%.c $(INC)
	mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -c $< -o $@


$(NAME) : $(OBJ) $(LIBFT)
	$(CC) -g -pthread $(OBJ) -o $(NAME) $(LIBFT)


clean:
	rm -fd $(OBJ) $(OBJDIR)


fclean:
	rm -fd $(OBJ) $(NAME) $(OBJDIR)


re:
	make fclean
	make all


.PHONY: clean fclean re all

.SILENT: