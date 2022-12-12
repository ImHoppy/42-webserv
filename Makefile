NAME		=	webserv

CC			=	c++

CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic-errors -I.
ifeq ($(DEBUG), 1)
	CFLAGS		+= -g3 -DDEBUG_LOG=1
endif

SRCS_DIR	=	srcs/
INCS_DIR	=	-Iincludes/
OBJS_DIR	=	obj/

SRCS		:=	main.cpp GeneralConfig.cpp ServerConfig.cpp \
				LocationConfig.cpp Parsing.cpp FillConfig.cpp Server.cpp \
				Utils.cpp Trim.cpp Client.cpp Request.cpp Response.cpp \
				Logger.cpp ResponseUtils.cpp CGI.cpp WebServ.cpp Time.cpp
SRCS		:=	$(addprefix $(SRCS_DIR), $(SRCS))
OBJS		=	$(patsubst $(SRCS_DIR)%.cpp, $(OBJS_DIR)%.o, $(SRCS))
DEPS		=	$(OBJS:.o=.d)

I = 0

ifndef MAX_I
MAX_I := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
		-nrRf $(firstword $(MAKEFILE_LIST)) MAX_I=1 compil="echo $(CC)" | grep -c '$(CC)')
endif

RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
BOLD=\033[1m
RESET=\033[0m

define compil
	@printf "$(RED)[%i/%i] (%i%%)\t$(BLUE)$(2)$(BOLD)$(3)$(RESET) " $(I) $(MAX_I) `expr $(I) \* 100 / $(MAX_I)`
	@$(1)
	@printf "\r%s\n" '$(1)'
	$(eval I = $(shell echo $$(($(I)+1))))
endef

$(OBJS_DIR)%.o :		$(SRCS_DIR)%.cpp
			@mkdir -p $(OBJS_DIR)
			$(call compil,$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCS_DIR),Compiling, $<)
				

$(NAME)		:	$(OBJS)
				$(call compil,$(CC) $(CFLAGS) $(OBJS) -o $(NAME),Linking, $@)
				@printf "$(RED)[%i/%i] (%i%%)\t$(GREEN)$(BOLD)$@$(RESET)$(GREEN) is ready!\n" $(I) $(MAX_I) `expr $(I) \* 100 / $(MAX_I)`

all			:	$(NAME)

clean		:
				rm -rf $(OBJS) $(DEPS)

fclean		:	clean
				rm -f $(NAME)

re			:	fclean
				@$(MAKE) --no-print-directory

debug		:	fclean
				@$(MAKE) --no-print-directory DEBUG=1

-include $(DEPS)

.DEFAULT_GOAL = all
.PHONY: all clean fclean re debug
