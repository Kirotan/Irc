NAME		= ircserv
CXXFLAGS	= -Wall -Wextra -Werror -g -Wshadow -std=c++98
CXX			= c++

SRC_DIR		= sources
OBJ_DIR		= object
INC_DIR		= includes

SRC_FILES	= sources/main.cpp \
				sources/Server.cpp \
				sources/Channel.cpp \
				sources/channelCommands.cpp \
				sources/Command.cpp \
				sources/display.cpp \
				sources/IrcMessage.cpp \
				sources/Parsing.cpp \
				sources/ServerIdentification.cpp \
				sources/serverMode.cpp \
				sources/ServerPingPong.cpp \
				sources/User.cpp


OBJ_FILES	= $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@$(CXX) $(OBJ_FILES) $(CXXFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)  # Crée le répertoire parent si nécessaire
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
