NAME		=	webserv

CC			=	c++

CFLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic-errors -I.
ifeq ($(DEBUG), 1)
	CFLAGS		+= -g3 -DDEBUG_LOG=1
endif

SRCS		=	main.cpp GeneralConfig.cpp ServerConfig.cpp \
				LocationConfig.cpp Parsing.cpp FillConfig.cpp \
				Utils.cpp Trim.cpp Request.cpp AEntity.cpp
# Request.cpp Response.cpp \
# Utils.cpp Server.cpp

OBJS		=	$(SRCS:.cpp=.o)
DEPS		=	$(OBJS:.o=.d)

RQST_NAME = rqst
RQST_SRCS = request_tests.cpp Request.cpp
RQST_HDRS = Request.hpp
RQST_OBJS = $(RQST_SRCS:.cpp=.o)
RQST_DEPS = $(RQST_OBJS:.o=.d)

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

%.o			:	%.cpp
				$(call compil,$(CC) $(CFLAGS) -MMD -MP -c $< -o $@,Compiling, $<)
				

$(NAME)		:	$(OBJS)
				$(call compil,$(CC) $(CFLAGS) $(OBJS) -o $(NAME),Linking, $@)
				@printf "$(RED)[%i/%i] (%i%%)\t$(GREEN)$(BOLD)$@$(RESET)$(GREEN) is ready!\n" $(I) $(MAX_I) `expr $(I) \* 100 / $(MAX_I)`
$(RQST_NAME)	:	$(RQST_OBJS)
				$(CC) $(CFLAGS) $(CFLAGS) -o $(RQST_NAME) $(RQST_OBJS)
				

all			:	$(NAME)

clean		:
				rm -rf $(OBJS) $(DEPS) $(RQST_OBJS) $(RQST_DEPS)

fclean		:	clean
				rm -f $(NAME)

re			:	fclean
				@$(MAKE) --no-print-directory

debug		:	fclean
				@$(MAKE) --no-print-directory DEBUG=1

-include $(DEPS)
-include $(RQST_DEPS)

.DEFAULT_GOAL = all
.PHONY: all clean fclean re debug
