# ============================================================================ #
#                                   VARIABLES                                  #
# ============================================================================ #

NAME		= ircserv

# Compilateur et flags
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -I./includes

# Répertoires
SRC_DIR		= src
OBJ_DIR		= objs
INC_DIR		= includes

# Fichiers sources
SRCS		= main.cpp \
				$(SRC_DIR)/Server.cpp \
				$(SRC_DIR)/Client.cpp \
				$(SRC_DIR)/Channel.cpp \
				$(SRC_DIR)/Command.cpp \
				$(SRC_DIR)/MessageParser.cpp \
				$(SRC_DIR)/PassCommand.cpp \
				$(SRC_DIR)/NickCommand.cpp \
				$(SRC_DIR)/UserCommand.cpp \
				$(SRC_DIR)/JoinCommand.cpp \
				$(SRC_DIR)/PartCommand.cpp \
				$(SRC_DIR)/PrivmsgCommand.cpp \
				$(SRC_DIR)/KickCommand.cpp \
				$(SRC_DIR)/InviteCommand.cpp \
				$(SRC_DIR)/TopicCommand.cpp \
				$(SRC_DIR)/ModeCommand.cpp \
				$(SRC_DIR)/QuitCommand.cpp

# Fichiers objets
OBJS		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Couleurs pour l'affichage
GREEN		= \033[0;32m
RED			= \033[0;31m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
RESET		= \033[0m

# ============================================================================ #
#                                   RÈGLES                                     #
# ============================================================================ #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)Linking $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)✓ Object files cleaned!$(RESET)"

fclean: clean
	@echo "$(RED)Cleaning $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ $(NAME) cleaned!$(RESET)"

re: fclean all

# ============================================================================ #
#                                   PHONY                                      #
# ============================================================================ #

.PHONY: all clean fclean re
