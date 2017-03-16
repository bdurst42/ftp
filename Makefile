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
		send_files.c \
		get_files.c \
		check.c \
		nmatch.c \
		cmd.c \
		dir.c \
		args.c \
		wildcard.c

OBJS1 = $(patsubst %.c, $(DIR_OBJ)%.o, $(FILES)) $(patsubst %.c, $(DIR_OBJ)%.o, $(MAIN1))

OBJS2 = $(patsubst %.c, $(DIR_OBJ)%.o, $(FILES)) $(patsubst %.c, $(DIR_OBJ)%.o, $(MAIN2))

DEP_OBJ1 = src_msg \
		  $(OBJS1)

DEP_OBJ2 = src_msg \
		  $(OBJS2)

RM = /bin/rm -rf

all: serveur client

serveur: mkdirobj $(DEP_OBJ1)
		@ make -C $(DIR_LIB)
		@ /bin/echo -n "Archiving object in $(NAME1):"
		@ $(CC) -o $@ $(OBJS1) $(MFLAGS) -L $(DIR_LIB) -lft
		@ echo " \033[32mAll done!\033[0m"

client: mkdirobj $(DEP_OBJ2)
		@ make -C $(DIR_LIB)
		@ /bin/echo -n "Archiving object in $(NAME2):"
		@ $(CC) -o $@ $(OBJS2) $(MFLAGS) -L $(DIR_LIB) -lft
		@ echo " \033[32mAll done!\033[0m"

$(DIR_OBJ)%.o: $(SRC_DIR)%.c
	@ /bin/echo -n "    $(notdir $@)"
	@ $(CC) $(CFLAGS) -c -o $@ $< -I $(INCLUDE_DIR)
	@ echo " \033[32mOK\033[0m"

mkdirobj:
	@ mkdir -p $(DIR_OBJ)

clean:
	@ make clean -C $(DIR_LIB)
	@ /bin/echo -n "Removing object files:"
	@ $(RM) $(DIR_OBJ)
	@ echo " \033[32mdone\033[0m"

fclean: clean
	@ make fclean -C $(DIR_LIB)
	@ /bin/echo -n "Removing library:"
	@ $(RM) serveur
	@ $(RM) client
	@ echo " \033[32mdone\033[0m"

re: fclean all

src_msg:
	@ echo " src functions:"

.PHONY : all clean fclean re client serveur
