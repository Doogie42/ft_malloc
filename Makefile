SRCFILES=malloc.c dump_malloc.c search.c free.c calloc.c realloc.c zone.c

SRCSPRINT=$(wildcard srcs/printf/*.c)
SRCSRAW=$(notdir $(SRCSPRINT))
SRCFILES+=$(SRCSRAW)
OBJSRC=$(SRCFILES:.c=.o)
OBJDIR=obj

SRCDIR=srcs
SRC=$(addprefix $(SRCDIR)/, $(SRCFILES))
OBJ=$(addprefix $(OBJDIR)/, $(OBJSRC))

NAME=libft_malloc.so
FLAG=-Wall -Wextra -Wpedantic -fvisibility=hidden -g
vpath %.c srcs srcs/printf

all: $(NAME)


$(NAME): $(OBJ)
	cc -shared -fPIC $(FLAG)  $(OBJ) -o $(NAME)

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

SRC_TEST_RAW=test0.c test1.c test2.c test3.c test4.c test3bis.c
DIR_TEST=bench/test/
SRC_TEST=$(addprefix $(DIR_TEST), $(SRC_TEST_RAW))

EXEC_TEST=$(SRC_TEST:.c=)

compiletest: copy $(EXEC_TEST)

copy:
	cp libft_malloc.so bench/test
$(EXEC_TEST): %: %.c
	cc $(CFLAGS) -o $@ $<

.PHONY: all clean fclean re