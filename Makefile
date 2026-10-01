#==================================== MAKEFILE ====================================#

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

INCLUDES = -I includes \
		   -I includes/server \
		   -I includes/Http \
		   -I includes/Utils \
# 		   -Iincludes/json

SRCS_DIR = srcs
JSON_DIR = $(SRCS_DIR)/json
SERV_DIR = $(SRCS_DIR)/server
HTTP_DIR = $(SRCS_DIR)/Http
UTILS_DIR = $(SRCS_DIR)/Utils

SRCS = main.cpp \
	$(HTTP_DIR)/HttpParser.cpp \
	$(HTTP_DIR)/HttpException.cpp \
	$(HTTP_DIR)/HttpResponse.cpp \
	$(HTTP_DIR)/HttpRequestHandler.cpp \
	$(SERV_DIR)/Server.cpp \
	$(SERV_DIR)/Client.cpp \
	$(UTILS_DIR)/Utils.cpp \
# 	$(JSON_DIR)/json/JsonLexer.cpp \
	$(JSON_DIR)/json/JsonValue.cpp \
	$(JSON_DIR)/json/JsonParser.cpp

OBJS_DIR = objs

OBJS = $(SRCS:%.cpp=$(OBJS_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

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

.PHONY: all clean fclean re

#==================================== RULES ====================================#

all: header $(NAME)
	@printf "$(PRIMARY)Build Finished !$(RESET)\n"

header:
	$(HEADER)
	@printf "\n"

$(NAME): $(OBJS)
	@printf "\n"
	@printf "$(INFO)Linking %s...$(RESET)\n" "$(NAME)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@printf "$(PRIMARY)%s Ready !$(RESET)\n" "$(NAME)"

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "\r$(ACCENT)Compiling: %-40s$(RESET)$(ESC)[K" "$<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

clean: header
	@printf "$(INFO)Cleaning $(NAME) objects...$(RESET)\n"
	@rm -rf $(OBJS_DIR)
	@printf "\r$(PRIMARY)$(NAME) objects cleaned!$(RESET)$(ESC)[K\n"

fclean: header clean
	@printf "$(ERROR)Cleanning %s...$(RESET)\n" "$(NAME)"
	@rm -rf $(NAME)

re: fclean all