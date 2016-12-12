NAME1 = server

NAME2 = client

CC = gcc

CFLAGS = -Wall -Wextra -Werror

MFLAGS =

INCLUDE_DIR = ./includes

DIR_OBJ = ./obj/

DIR_LIB = ./libft/

SRC_DIR = ./srcs/

MAIN1 = server.c

MAIN2 = client.c

FILES = package.c \
		manage_file.c \
		check.c

OBJS1 = $(patsubst %.c, $(DIR_OBJ)%.o, $(FILES)) $(patsubst %.c, $(DIR_OBJ)%.o, $(MAIN1))

OBJS2 = $(patsubst %.c, $(DIR_OBJ)%.o, $(FILES)) $(patsubst %.c, $(DIR_OBJ)%.o, $(MAIN2))

DEP_OBJ1 = src_msg \
		  $(OBJS1)

DEP_OBJ2 = src_msg \
		  $(OBJS2)

RM = /bin/rm -rf

all: $(NAME1) $(NAME2)

$(NAME1): mkdirobj $(DEP_OBJ1)
		@ /bin/echo -n "Archiving object in $(NAME1):"
		@ $(CC) -o $(NAME1) $(OBJS1) $(MFLAGS) -L $(DIR_LIB) -lft
		@ echo " \033[32mAll done!\033[0m"

$(NAME2): mkdirobj $(DEP_OBJ2)
		@ /bin/echo -n "Archiving object in $(NAME2):"
		@ $(CC) -o $(NAME2) $(OBJS2) $(MFLAGS) -L $(DIR_LIB) -lft
		@ echo " \033[32mAll done!\033[0m"

$(DIR_OBJ)%.o: $(SRC_DIR)%.c
	@ /bin/echo -n "    $(notdir $@)"
	@ $(CC) $(CFLAGS) -c -o $@ $< -I $(INCLUDE_DIR)
	@ echo " \033[32mOK\033[0m"

mkdirobj:
	@ mkdir -p $(DIR_OBJ)

clean:
	@ /bin/echo -n "Removing object files:"
	@ $(RM) $(DIR_OBJ)
	@ echo " \033[32mdone\033[0m"

fclean: clean
	@ /bin/echo -n "Removing library:"
	@ $(RM) $(NAME1)
	@ $(RM) $(NAME2)
	@ echo " \033[32mdone\033[0m"

re: fclean all

src_msg:
	@ echo " src functions:"

.PHONY : all clean fclean re
