NAME		=	webserv

CXX			=	clang++
CXXFLAGS	=	-Wall -Wextra -Werror -Wpedantic -Wsign-conversion -std=c++98

RM			=	rm -rf
MKDIR		=	mkdir -p

SRC_DIR		=	src
INC_DIR		=	include
OBJ_DIR		=	obj
INCS		=	$(wildcard $(INC_DIR)/*.hpp)
SRCS		=	main.cpp Server.cpp Request.cpp RequestLine.cpp
OBJS		=	$(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

VPATH		=	$(SRC_DIR)

all:			CXXFLAGS += -g -fno-limit-debug-info
all:			$(NAME)

release:		CXXFLAGS += -O3
release:		$(NAME)

$(OBJ_DIR)/%.o:	%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(NAME):		$(OBJ_DIR) $(OBJS)
	$(CXX) $(OBJS) -o $(NAME)

$(OBJ_DIR):
	@$(MKDIR) $(OBJ_DIR)

clean:
	@$(RM) $(OBJS)

fclean:		clean
	@$(RM) $(NAME)

re:			fclean all

fmt:
	clang-format -i $(addprefix $(SRC_DIR)/, $(SRCS)) $(INCS)

.PHONY:		all release clean fclean re fmt
