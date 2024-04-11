#include "Server.hpp"

Server::Server()
{
	std::cout << "---------- FT_IRC SERVER ----------" << std::endl;
	this->sockfd = -1;
	opt_val = 1;
	poll_size = 10;
	max_client = 10;
}

Server::Server(Server const &obj)
{
	*this = obj;
}

Server &Server::operator=(Server const &obj){
	if (this != &obj)
	{
		this->sockfd = obj.sockfd;
		this->port = obj.port;
		this->password = obj.password;
		this->sockclient = obj.sockclient;
		this->poll_fd = obj.poll_fd;
	}
	return *this;
}

Server::~Server()
{
	std::cout << "---------- FT_IRC CLOSED ----------" << std::endl;
}

int	Server::getSockfd()
{
	return (this->sockfd);
}

int	Server::getPort()
{
	return (this->port);
}

int	Server::getFdByNick(std::string nickname)
{
	int	fd;

	for (size_t i = 0; i < sockclient.size(); i++)
	{
		if (nickname == sockclient[i].getNickname())
		{
			fd = sockclient[i].getFduser();
			return (fd);
		}
	}
	return (-1);
}

std::string	Server::getPassword()
{
	return (this->password);
}

User	*Server::getClientFduser(int fd)
{
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		if (this->sockclient[i].getFduser() == fd)
			return (&this->sockclient[i]);
	}
	return (NULL);
}

User	*Server::getClientNickname(std::string nickname)
{
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		if (this->sockclient[i].getNickname() == nickname)
			return (&this->sockclient[i]);
	}
	return (NULL);
}

Channel	*Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->channel.size(); i++)
	{
		if (this->channel[i].getChannelName() == name)
			return (&channel[i]);
	}
	return (NULL);
}

void	Server::setSockfd(int fd)
{
	this->sockfd = fd;
}

void	Server::setPort(int port)
{
	this->port = port;
}

void	Server::setPassword(std::string password)
{
	this->password = password;
}

void	Server::setClientUser(User newuser)
{
	this->sockclient.push_back(newuser);
}

void	Server::setChannel(Channel newchannel)
{
	this->channel.push_back(newchannel);
}

void	Server::setPollfd(pollfd fd)
{
	this->poll_fd.push_back(fd);
}

void	Server::removeClientUser(int fd){
	for (size_t i = 0; i < this->sockclient.size(); i++)
	{
		if (this->sockclient[i].getFduser() == fd)
		{
			this->sockclient.erase(this->sockclient.begin() + i);
			return ;
		}
	}
}

void	Server::removeFd(int fd){
	for (size_t i = 0; i < this->poll_fd.size(); i++)
	{
		if (this->poll_fd[i].fd == fd)
		{
			this->poll_fd.erase(this->poll_fd.begin() + i);
			return;
		}
	}
}

// parcourir tous les canaux du serveur 
// et retirer les users de chaque canal
// puis supprimer le canal vide
void	Server::clearChannel(int fd)
{
	int	flag; // utilisé pour suivre si l'utilisateur a été retiré d'un canal
	std::string	reply;

	for (size_t i = 0; i < this->channel.size(); i++)
	{
		flag = 0;
		if (channel[i].getUserFd(fd))
		{
			channel[i].removeUser(fd);
			flag = 1;
		}
		else if (channel[i].getOpFd(fd))
		{
			channel[i].removeOp(fd);
			flag = 1;
		}
		if (channel[i].numClient() == 0)
		{
			channel.erase(channel.begin() + i);
			i--;
			continue;
		}
		if (flag)
		{
			reply = ":" + getClientFduser(fd)->getNickname() + "!~" + getClientFduser(fd)->getUser() + "@localhost QUIT Quit\r\n";
			channel[i].sendAll(reply);
		}
	}
}

// send() permet d'envoyer des données au socket fd, souvent utilisé pour envoyer des messages provenant de l'execution des commandes
void	Server::sendMessage(std::string message, int fd)
{
	std::cout << ">> " << message;
	if (send(fd, message.c_str(), message.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}

void	Server::sendMessage2(int errnum, std::string user, std::string channel, int fd, std::string message)
{
	std::stringstream ss;
	std::string	rep;

	ss << ":localhost " << errnum << " " << user << " " << channel << message;
	rep = ss.str();
	if (send(fd, rep.c_str(), rep.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}

void	Server::sendMessage3(int errnum, std::string user, int fd, std::string message)
{
	std::stringstream ss;
	std::string	rep;

	ss << ":localhost " << errnum << " " << user << " " << message;
	rep = ss.str();
	if (send(fd, rep.c_str(), rep.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}

// fermer les fd des users et du serveur
void	Server::closeFd()
{
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		std::cout << "FD[" << sockfd << "] disconnected" << std::endl;
		close(sockclient[i].getFduser());
	}
	if (sockfd != -1)
	{
		std::cout << "Server (FD[" << sockfd << "]) disconnected" << std::endl;
		close(sockfd);
	}
}

bool	Server::isValidArg(std::string arg)
{
	return (arg.find_first_not_of("0123456789") == std::string::npos && \
	atoi(arg.c_str()) >= 1024 && atoi(arg.c_str()) <= 65535);
}

bool	Server::isRegistered(int fd)
{
	if (!getClientFduser(fd) || getClientFduser(fd)->getNickname().empty() || getClientFduser(fd)->getUser().empty() || getClientFduser(fd)->getNickname() == "*" || !getClientFduser(fd)->getConnected())
		return (false);
	return (true);
}

bool	Server::checkChannelExist(std::string channelName)
{
	if (channelName.empty() || channelName.size() <= 2 || channelName[0] != '#')
		return (false);
	
	std::string chan = channelName.substr(1); //ChannelName without #
	for (std::vector<Channel>::iterator it = channel.begin(); it != channel.end(); ++it)
	{
		if (it->getChannelName() == chan)
			return (true);
	}
	return (false);
}
