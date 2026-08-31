#==================================== MAKEFILE ====================================#

NAME = webserv

CC = c++

#-Wall -Werror -Wextra -std=c++98 -g \		

FLAGS = -I includes \
		-I includes/server \
# 		-I includes/json \

SRCS_DIR = srcs

SRCS = main.cpp \
	$(SRCS_DIR)/server/Client.cpp \
	$(SRCS_DIR)/server/Server.cpp
# 	$(SRCS_DIR)/json/JsonLexer.cpp \
	$(SRCS_DIR)/json/JsonValue.cpp \
	$(SRCS_DIR)/json/JsonParser.cpp \

OBJS_DIR = objs

OBJS = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)

#==================================== COLORS ====================================#

ESC		= \033
RESET   = $(ESC)[0m
BOLD    = $(ESC)[1m

PRIMARY = $(ESC)[38;2;179;71;80m
INFO    = $(ESC)[38;2;229;163;168m
ACCENT  = $(ESC)[38;2;217;140;60m
WARN    = $(ESC)[38;2;196;98;106m
ERROR   = $(ESC)[38;2;204;40;40m

#==================================== HEADER ====================================#

define HEADER
@printf "$(ESC)[38;2;229;163;168m █████   ███   █████          █████      █████████                                $(RESET)\n"
@printf "$(ESC)[38;2;212;130;137m░░███   ░███  ░░███          ░░███      ███░░░░░███                               $(RESET)\n"
@printf "$(ESC)[38;2;196;98;106m ░███   ░███   ░███   ██████  ░███████ ░███    ░░░   ██████  ████████  █████ █████$(RESET)\n"
@printf "$(ESC)[38;2;179;71;80m ░███   ░███   ░███  ███░░███ ░███░░███░░█████████  ███░░███░░███░░███░░███ ░░███ $(RESET)\n"
@printf "$(ESC)[38;2;153;63;69m ░░███  █████  ███  ░███████  ░███ ░███ ░░░░░░░░███░███████  ░███ ░░░  ░███  ░███ $(RESET)\n"
@printf "$(ESC)[38;2;128;53;58m  ░░░█████░█████░   ░███░░░   ░███ ░███ ███    ░███░███░░░   ░███      ░░███ ███  $(RESET)\n"
@printf "$(ESC)[38;2;102;42;46m    ░░███ ░░███     ░░██████  ████████ ░░█████████ ░░██████  █████      ░░█████   $(RESET)\n"
@printf "$(ESC)[38;2;77;31;34m     ░░░   ░░░       ░░░░░░  ░░░░░░░░   ░░░░░░░░░   ░░░░░░  ░░░░░        ░░░░░    $(RESET)\n"
endef

#==================================== PHONY ====================================#

.PHONY: all bonus clean fclean re

#==================================== RULES ====================================#

all: header $(NAME)
	@printf "$(PRIMARY)Build Finished !$(RESET)\n"

header:
	$(HEADER)
	@printf "\n";

$(NAME): $(OBJS)
	@printf "\n"
	@printf "$(INFO)Linking %s...$(RESET)\n" "$(NAME)"
	@$(CC) $(OBJS) -o $(NAME)
	@printf "$(PRIMARY)%s Ready !$(RESET)\n" "$(NAME)"

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "\r$(ACCENT)Compiling: %-40s$(RESET)$(ESC)[K" "$<"
	@$(CC) $(FLAGS) -c $< -o $@

clean: header
	@printf "$(INFO)Cleaning $(NAME) objects...$(RESET)\n"
	@rm -rf $(OBJS_DIR)
	@printf "\r$(PRIMARY)$(NAME) objects cleaned!$(RESET)$(ESC)[K\n"

fclean: header clean
	@printf "$(ERROR)Cleanning %s...$(RESET)\n" "$(NAME)"
	@rm -rf $(NAME)

re: header fclean all
