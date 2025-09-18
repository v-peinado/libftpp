#Style
GREEN	=	\033[92;5;118m
YELLOW	=	\033[93;5;226m
GRAY	=	\033[33;2;37m
RESET	=	\e[0m
CURSIVE	=	\033[33;3m
RED		= 	\033[31m

#Program name
NAME	= libftpp.a

#Compiler
CC		= c++
CFLAGS	= -std=c++20 -Wall -Wextra -Werror
#CFLAGS	+= -g3 -fsanitize=address
RM		= rm -f

#Includes
INC		= -I ./ -I ./inc/

#Source files
SRC_DIR	= src/
SRC 	= $(shell find $(SRC_DIR) -type f -iname "*.cpp" | sed 's|^src/||')

#Object files
OBJ_DIR	= obj/
OBJ		= $(SRC:.cpp=.o)
OBJS 	= $(addprefix $(OBJ_DIR), $(OBJ))

all: obj $(NAME)

obj:
	@rsync -av --include '*/' --exclude '*' --quiet $(SRC_DIR) $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

$(NAME): $(OBJS)
	@ar rcs $(NAME) $(OBJS)
	@printf "$(GREEN)$(NAME): Static library created successfully!$(RESET)\n"

clean:
	@$(RM) -Rf $(OBJ_DIR)
	@printf "$(YELLOW)Object files: $(RED)Removed!$(RESET)\n"

fclean: clean
	@$(RM) -f $(NAME)
	@printf "$(YELLOW)$(NAME): $(RED)Removed!$(RESET)\n"

re: fclean all

.PHONY: all re clean fclean