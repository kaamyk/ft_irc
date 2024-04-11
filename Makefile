NAME = ircserv
CPP = c++
CFLAGS = -Wall -Wextra -Werror -g3
CPPFLAGS = -std=c++98

SRCS =	main.cpp \
		Server/Server.cpp \
		Server/ServerInit.cpp \
		Server/ServerParsing.cpp \
		User/User.cpp \
		Channel/Channel.cpp \
		Command/PASS.cpp \
		Command/NICK.cpp \
		Command/USER.cpp \
		Command/WHOIS.cpp \
		Command/WHO.cpp \
		Command/OPER.cpp \
		Command/QUIT.cpp \
		Command/PING.cpp \
		Command/JOIN.cpp \
		Command/PART.cpp \
		Command/KICK.cpp \
		Command/MODE_USER.cpp \
		Command/MODE_CHANNEL.cpp \
		Command/TOPIC.cpp \
		Command/NOTICE.cpp \
		Command/INVITE.cpp \
		Command/AWAY.cpp \
		Command/SENDMSG.cpp \
		Command/PRIVMSG.cpp

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
