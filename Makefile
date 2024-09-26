SRCFILES=malloc.c

SRCSPRINT=$(wildcard srcs/ft_printf/*.c)
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
vpath %.c srcs srcs/ft_printf

all: $(NAME)


$(NAME): $(OBJ)
	cc -shared $(FLAG)  $(OBJ) -o $(NAME)
	cp $(NAME) test/

obj/%.o: %.c
	mkdir -p obj
	cc $(FLAG) -fPIC -I srcs/ft_printf -c $< -o  $@

# OBJMAIN=obj/main.o obj/malloc.o

# obj/%.o: srcs/%.c
# 	mkdir -p obj
# 	cc $(FLAG)  -I srcs/ft_printf -c $< -o $@

main: $(OBJMAIN)
	cc $(FLAG) -I srcs/ft_printf $(OBJMAIN) -o main libft/printf/libftprintf.a
 
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