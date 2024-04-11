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
#include <sys/socket.h> // for socket()
#include <sys/types.h> // for socket()
#include <netinet/in.h> // for sockaddr_in
#include <fcntl.h> // for fcntl() in mac
#include <unistd.h> // for close()
#include <arpa/inet.h> // for inet_ntoa()
#include <poll.h> // for poll()
#include <csignal> // for signal()
#include "../User/User.hpp"
#include "../Channel/Channel.hpp"
#include "../Command/Message.hpp"

class User;
class Channel;

class Server
{
private:

	int	sockfd;
	int	port;
	std::string	password;
	int	poll_size;
	int	status;
	int	max_client;
	int opt_val;
	int	addr_len;
	static bool signal;
	struct pollfd	new_client;
	std::vector<struct pollfd>	poll_fd;
	std::vector<User>	sockclient;
	std::vector<Channel>	channel;
	std::map<int, std::string>	buffer;
	int	(Server::*parse[8])(std::string split_mess[3]);

public:

	Server();
	Server(Server const &obj);
	Server &operator=(Server const &obj);
	~Server();

	// Getters
	int	getSockfd();
	int	getPort();
	int	getFdByNick(std::string nickname);
	std::string	getPassword();
	User	*getClientFduser(int fd);
	User	*getClientNickname(std::string nickname);
	Channel	*getChannel(std::string name);

	// Setters
	void	setSockfd(int sockfd);
	void	setPort(int port);
	void	setPassword(std::string password);
	void	setClientUser(User newuser);
	void	setChannel(Channel newchannel);
	void	setPollfd(pollfd fd);

	// Removers
	void	removeClientUser(int fd);
	void	removeFd(int fd);
	void	clearChannel(int fd);

	// Send Methods
	void	sendMessage(std::string message, int fd);
	void	sendMessage2(int errnum, std::string user, std::string channel, int fd, std::string message);
	void	sendMessage3(int errnum, std::string user, int fd, std::string message);

	// Utils Methods
	void	closeFd();
	bool	isValidArg(std::string port);
	bool	isRegistered(int fd);
	bool	checkChannelExist(std::string channelName);

	// ServerInit Methods
	void	initServer(int port, std::string pass);
	void	checkPoll();
	void	acceptClient();
	void	receiveEvent(int fd);
	static void	signalHandler(int signum);

	// ServerParsing Methods
	int	splitMessage( std::string message, std::string split_mess[3] );
	int	splitParams( std::string params, std::string split_params[3] );
	int	parseNick( std::string split_mess[3] );
	int	parseUser( std::string split_mess[3] );
	int	parseJoin( std::string split_mess[3] );
	int	parseTopic( std::string split_mess[3], std::string split_params[3] );
	int	parseMode( std::string split_mess[3] );
	int	parseKick( std::string split_mess[3] );
	int	parseInvite( std::string split_mess[3] );
	int8_t	parseCommand( std::string command );
	std::vector<std::string>	splitParam(std::string &message);
	std::vector<std::string>	parseMessage( std::string &message );
	std::vector<std::string>	splitBuffer(std::string buffer);
	void	parseCommandList(std::string &command, int fd);

	// Command Methods
	void	PASS(std::string message, int fd);

	void	NICK(std::string message, int fd);
	bool	usedNickname(std::string &nickname);
	bool	validNickname(std::string &nickname);
	void	updateChannel(std::string old, std::string n_nick);

	void	USER(std::string &message, int fd);

	void	QUIT(std::string message, int fd);
	std::string	quitReason(std::string message);
	void	quitFormatReason(std::string message, std::string str, std::string &reason);

	void	WHOIS(std::string message, int fd);
	std::string	idle(time_t start);
	
	void	PING(std::string &message, int fd);

	void	JOIN(std::string message, int fd);
	int	splitJoin(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd);
	void	addToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i , int j, int fd);
	void	addToNewChannel(std::vector<std::pair<std::string, std::string> >&param, int i, int fd);
	int	countJoinedChannel(std::string user);
	bool	isInvited(User *user, std::string channel, int flag);

	void	INVITE(std::string message, int fd);

	void	WHO(std::string message, int fd);
	std::string	whoFlag(User *user, Channel *chan);

	void	NOTICE(std::string message, int fd);
	std::vector<std::string>	splitNotice(std::string str);
	void	chanNotice(User *user, std::string target, std::string msg);
	void	userNotice(User *user, std::string target, std::string msg);

	void	PART(std::string message, int fd);
	int	splitPart(std::string message, std::vector<std::string> &param, std::string &reason, int fd);
	std::string	splitPartReason(std::string &message, std::vector<std::string> &param);
	void	findReason(std::string message, std::string tofind, std::string &reason);

	void	KICK(std::string message, int fd);
	std::string	splitKick(std::string message, std::vector<std::string> &param, std::string &user, int fd);
	std::string	splitKickReason(std::string &message, std::vector<std::string> &param);
	void	findKickReason(std::string message, std::string tofind, std::string &comment);

	void	OPER(std::string &message, int fd);

	void	TOPIC(std::string message, int fd);

	void	MODE_CHANNEL(std::string &message, int fd);
	void	parseModeChan(std::string message, std::string &channelname, std::string &modestring, std::string &param);
	std::vector<std::string>	splitModeChan(std::string param);
	std::string	updateAddMinus(std::string ssmode, char addminus, char mode);
	std::string	modeInviteOnly(Channel *channel, char addminus, std::string ssmode);
	std::string	modeTopicRest(Channel *channel, char addminus, std::string ssmode);
	std::string	modePassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
	bool	validModePassword(std::string password);
	std::string	modeOpPrivi(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
	std::string	modeLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
	bool	validModeLimit(std::string &limit);

	void	MODE_USER(std::string &message, int fd);
	void	parseModeUser(std::string message, std::string &nickname, std::string &modestring, std::string &param);
	std::vector<std::string>	splitModeUser(std::string param);
	std::string	modeUserAppend(std::string ssmode, char addminus, char mode);
	std::string	modeInvisible(User *user, char addminus, std::string ssmode);
	std::string	modeOperator(User *user, char addminus, std::string ssmode);

	void	AWAY(std::string message, int fd);

	void	SENDMSG(std::string message, int fd);
	void	CheckForChannels_Clients(std::vector<std::string> &tmp, int fd);

	bool	checkOption( std::string opt );
	bool	checkChannel( std::string ch );
	bool	checkPrefix( std::string pref );
	std::string	getTargetCh(std::string &target);
	std::string	getTargetUs(int fd);
	std::vector<std::string>	splitRecipients(std::string raw_rec);
	bool	parseRecipient(std::string recipient, int fd);
	int8_t	parsePrivmsgSyntax( std::string split_mess[3], std::string split_params[3] );
	int8_t	parsePrivmsg(std::string split_message[3], std::string split_params[3], int fd);
	void	PRIVMSG(std::string &message, int fd);
};
