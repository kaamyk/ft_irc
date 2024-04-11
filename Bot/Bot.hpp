#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <netdb.h>
#include <sys/socket.h> // for socket()
#include <sys/types.h> // for socket()
#include <netinet/in.h> // for sockaddr_in
#include <fcntl.h> // for fcntl() in mac
#include <unistd.h> // for close()
#include <arpa/inet.h> // for inet_ntoa()
#include <poll.h> // for poll()
#include <csignal> // for signal()

class Bot
{
private:
	static bool	sigflag;
	std::string	addr;
	std::string	pass;
	std::string	botname;

	int	sockfd;
	struct sockaddr_in	serv;

	std::string	rec;
	std::vector<std::string>	cmd;
	bool	connected;
	int	pingpong;

public:
	Bot(std::string addr, int port, std::string pass, std::string botname);
	Bot(const Bot &other);
	Bot& operator=(const Bot &other);
	~Bot();

	void	botStart();
	void	recv_cmd();
	std::vector<std::string>	splitMsg(std::string msg);
	std::string	toRoll(size_t n);

	void	sendMessage(std::string message);
	static void	signalHandler(int signum);
};
