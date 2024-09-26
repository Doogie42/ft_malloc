SRCFILES=malloc.c

SRCSPRINT=$(wildcard srcs/printf/*.c)
# SRCSRAW=$(notdir srcs/ft_printf/ft_printf.c)
SRCSRAW=$(notdir $(SRCSPRINT))
SRCFILES+=$(SRCSRAW)
OBJSRC=$(SRCFILES:.c=.o)
OBJDIR=obj

SRCDIR=srcs
SRC=$(addprefix $(SRCDIR)/, $(SRCFILES))
OBJ=$(addprefix $(OBJDIR)/, $(OBJSRC))

NAME=libft_malloc.so
FLAG=-Wall -Wextra
vpath %.c srcs srcs/printf

all: $(NAME)


$(NAME): $(OBJ)
	cc -shared $(FLAG)  $(OBJ) -o $(NAME)
	cp $(NAME) test/

obj/%.o: %.c
	mkdir -p obj
	cc $(FLAG) -fPIC -I srcs/printf -c $< -o  $@

OBJMAIN=obj/main.o
OBJMAIN+=$(OBJ)

main: $(OBJMAIN)
	cc $(FLAG) -I srcs/printf $(OBJMAIN) -o main
 
clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

SRC_TEST=test1.c test0.c
EXEC_TEST=$(SRC_TEST:.c=)



compiletest: $(EXEC_TEST)

$(EXEC_TEST): %: test/%.c
	$(CC) $(CFLAGS) -o test/$@ $<



.PHONY: all clean fclean re