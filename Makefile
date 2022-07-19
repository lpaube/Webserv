NAME		=	webserv

CXX			=	clang++
CXXFLAGS	=	-Wall -Wextra -Werror -Wpedantic -Wsign-conversion  -std=c++98

RM			=	rm -rf
MKDIR		=	mkdir -p

SRC_DIR		=	src
OBJ_DIR		=	obj
DIRS		=	$(shell find $(SRC_DIR) -type d)
INCS		=	$(shell find $(SRC_DIR) -type f -name "*.hpp")
SRCS		=	$(shell find $(SRC_DIR) -type f -name "*.cpp")
SRCS_BASE	=	$(foreach file, $(SRCS), $(shell basename -a $(file)))
OBJS		=	$(addprefix $(OBJ_DIR)/, $(SRCS_BASE:.cpp=.o))

VPATH		=	$(SRC_DIR) $(DIRS)

all:			CXXFLAGS += -g -fno-limit-debug-info
all:			$(NAME)

release:		CXXFLAGS += -O3
release:		fclean $(NAME)

$(OBJ_DIR)/%.o:	%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c -o $@ $<

$(NAME):		$(OBJ_DIR) $(OBJS)
	$(CXX) -o $(NAME) $(OBJS)

$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

clean:
	@$(RM) $(OBJ_DIR)

fclean:			clean
	@$(RM) $(NAME)

re:				fclean all

fmt:
	clang-format -i $(SRCS) $(INCS)

.PHONY:			all release clean fclean re fmt
