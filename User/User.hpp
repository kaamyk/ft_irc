#pragma once

#include "../Server/Server.hpp"
#include "../Channel/Channel.hpp"

class Channel;

class User
{
private:
	int	fdUser;
	std::string	nickname;
	std::string	user;
	std::string ip;
	std::string buffer;
	std::string	realname;
	std::string awaymsg;
	bool	registered;
	bool	connected;
	bool	isOp;
	bool	isInvisible;
	bool	chanFounder;
	bool	isAway;
	time_t	created;
	time_t	idle;
	std::vector<std::string>	invitation;
	std::vector<std::pair<char, bool> >	modeUser;
public:
	User();
	User(int fd, std::string nickname, std::string user);
	User(User const &obj);
	User &operator=(User const &obj);
	bool operator==(User const &rhs);
	~User();

	int	getFduser();
	std::string	&getNickname();
	std::string	getUser();
	std::string	getIp();
	std::string	getBuffer();
	std::string getHostname();
	std::string	getModeUser();
	std::string	getRealname() const;
	bool	getRegistered();
	bool	getConnected();
	bool	getInvited(std::string &channel);
	bool	getOp();
	bool	getInvisible();
	bool	getModeUserOption(size_t i);
	bool	getChanFounder();
	time_t	getCreated() const;
	time_t	getIdle() const;
	bool	getisAway() const;
	const std::string	getAwayMessage() const;

	void	setFduser(int fd);
	void	setNickname(std::string &nickname);
	void	setUser(std::string &user);
	void	setIp(std::string ip);
	void	setBuffer(std::string recv);
	void	setRegistered(bool val);
	void	setConnected(bool val);
	void	setOp(bool op);
	void	setChanFounder(bool founder);
	void	setInvisible(bool inv);
	void	setModeUser(size_t i, bool mode);
	void	setRealname(std::string realname);
	void	setIdle();
	void	setAway(bool state, std::string msg);

	void	removeBuffer();
	void	addInvite(std::string &channel);
	void	removeInvite(std::string &channel);
};