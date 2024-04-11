NAME = botclient
CPP = c++
CFLAGS = -Wall -Wextra -Werror -g3
CPPFLAGS = -std=c++98

SRCS =	main.cpp \
		Bot.cpp

# OBJDIR = .obj
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(VFLAGS) -o $(NAME) $^

%.o : %.cpp
	$(CPP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re