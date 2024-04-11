#include "Channel.hpp"

/* ************************************************************************** */
/* 							CONSTRUCTORS / DESTRUCTORS						  */
/* ************************************************************************** */

Channel::Channel()
{
	this->_channelName = "";
	this->_topicname = "";
	this->created_at = "";
	this->topic = 0;
	this->key = 0;
	this->limit = 0;
	this->onlyInvited = 0;
	this->topicRest = false;
	char	mode[5] = {'i', 't', 'k', 'o', 'l'};
	for (int i = 0; i < 5; i++)
		modeChan.push_back(std::make_pair(mode[i], false));
}

Channel::Channel(Channel const &src)
{
	*this = src;
}

Channel::~Channel()
{
}

/* ************************************************************************** */
/* 									OPERATORS								  */
/* ************************************************************************** */

Channel	&Channel::operator=(Channel const &rhs)
{
	if (this != &rhs)
	{
		this->_channelName = rhs._channelName;
		this->_topicname = rhs._topicname;
		this->password = rhs.password;
		this->topic = rhs.topic;
		this->key = rhs.key;
		this->limit = rhs.limit;
		this->sockclient = rhs.sockclient;
		this->ops = rhs.ops;
		this->onlyInvited = rhs.onlyInvited;
		this->topicRest = rhs.topicRest;
		this->created_at = rhs.created_at;
		this->modeChan = rhs.modeChan;
	}
	return (*this);
}

/* ************************************************************************** */
/* 								MEMBER FUNCTIONS							  */
/* ************************************************************************** */

std::string	Channel::getChannelName()
{
	return (_channelName);
}

std::string	Channel::getTopicName()
{
	return (_topicname);
}

std::string	Channel::getChannelPass()
{
	return (this->password);
}

// générer une liste des users qui sont membres du canal qui sera envoyée en reply
std::string Channel::getChannelList()
{
	std::string	list;
	for (size_t i = 0; i < ops.size(); i++)
	{
		list += "@" + ops[i].getNickname();
		if ((i + 1) < ops.size())
			list += " ";
	}
	if (sockclient.size())
		list += " ";
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		list += sockclient[i].getNickname();
		if ((i + 1) < sockclient.size())
			list += " ";
	}
	return (list);
}

std::string	Channel::getCreatedAt()
{
	return (this->created_at);
}

std::string Channel::getModeChan()
{
	std::string	mode;
	for (size_t i = 0; i < modeChan.size(); i++)
	{
		if (modeChan[i].first != 'o' && modeChan[i].second)
			mode.push_back(modeChan[i].first);
	}
	if (!mode.empty())
		mode.insert(mode.begin(), '+');
	return (mode);
}

std::vector<User>	&Channel::getSockClient()
{
	return (sockclient);
}

std::vector<User>	&Channel::getSockOp()
{
	return (ops);
}

User	*Channel::getUserFd(int fd)
{
	for (std::vector<User>::iterator it = sockclient.begin(); it != sockclient.end(); ++it)
	{
		if (it->getFduser() == fd)
			return &(*it);
	}
	return (NULL);
}

User	*Channel::getOpFd(int fd)
{
	for (std::vector<User>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (it->getFduser() == fd)
			return &(*it);
	}
	return (NULL);
}

// rechercher un user dans sockclient et ops
User	*Channel::getFindUser(std::string name)
{
	for (std::vector<User>::iterator it = sockclient.begin(); it != sockclient.end(); ++it)
	{
		if (it->getNickname() == name)
			return &(*it);
	}
	for (std::vector<User>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (it->getNickname() == name)
			return &(*it);
	}
	return (NULL);
}

int	Channel::getOnlyInvited()
{
	return this->onlyInvited;
}

int	Channel::getTopic()
{
	return this->topic;
}

int	Channel::getKey()
{
	return this->key;
}

int	Channel::getLimit()
{
	return this->limit;
}

bool	Channel::getTopicRest() const
{
	return (this->topicRest);
}

bool	Channel::getModeChanOption(size_t i)
{
	return (modeChan[i].second);
}

std::vector<User *>	Channel::getUserList()
{
	std::vector<User*> pointers;
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		pointers.push_back(&sockclient[i]);
	}
	return (pointers);
}

void	Channel::setChannelName(std::string name)
{
	this->_channelName = name;
}

void	Channel::setTopicName(std::string topic)
{
	this->_topicname = topic;
}

void	Channel::setChannelPass(std::string password)
{
	this->password = password;
}

void	Channel::setOnlyInvited(int onlyInvited)
{
	this->onlyInvited = onlyInvited;
}

void	Channel::setTopicRest(bool restriction)
{
	this->topicRest = restriction;
}

void	Channel::setTopic(int topic)
{
	this->topic = topic;
}

void	Channel::setKey(int key)
{
	this->key = key;
}

void 	Channel::setLimit(int limit)
{
	this->limit = limit;
}

void	Channel::setModeChan(size_t i, bool mode)
{
	modeChan[i].second = mode;
}

// définir l'heure et stocké dans created_at (utilisée pour quand on crée un canal)
void	Channel::setCreatedAt()
{
	std::time_t _time = std::time(NULL);
	std::ostringstream	oss;
	oss << _time;
	this->created_at = std::string(oss.str());
}

void	Channel::removeUser(int fd)
{
	for (std::vector<User>::iterator it = sockclient.begin(); it != sockclient.end(); ++it)
	{
		if (it->getFduser() == fd)
		{
			sockclient.erase(it);
			break;
		}
	}
}

void	Channel::removeOp(int fd)
{
	for (std::vector<User>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (it->getFduser() == fd)
		{
			ops.erase(it);
			break;
		}
	}
}

void	Channel::addMember(User user)
{
	sockclient.push_back(user);
}

void	Channel::addChanOps(User user)
{
	ops.push_back(user);
}

void	Channel::checkChannelName(std::string channnelName)
{
	if (channnelName.empty() || channnelName.size() < 2)
		throw std::runtime_error("Incorrect channel name");

	if (channnelName[0] != '&')
		throw std::runtime_error("Incorrect channel name, it must begin with &");
	for (size_t i = 0; i < channnelName.size(); ++i)
	{
		if (channnelName[i] == ' ' || channnelName[i] == 0x07 || channnelName[i] == ',')
			throw std::runtime_error("Incorrect channel name");
	}
}

size_t	Channel::numClient()
{
	size_t	num;
	
	num = this->sockclient.size() + this->ops.size();
	return (num);
}

bool	Channel::isUserPresent(std::string &name)
{
	for(size_t i = 0; i < sockclient.size(); i++){
		if(sockclient[i].getNickname() == name)
			return true;
	}
	for(size_t i = 0; i < ops.size(); i++){
		if(ops[i].getNickname() == name)
			return true;
	}
	return false;
}

bool	Channel::isUserOp(std::string &name)
{
	for (std::vector<User>::iterator it = ops.begin(); it != ops.end(); ++it)
	{
		if (it->getNickname() == name)
			return (true);
	}
	return (false);
}

bool	Channel::haveOp()
{
	if (ops.empty())
		return (false);
	return (true);
}

// envoyer un message à tous les users et opérateurs présents dans le canal
void	Channel::sendAll(std::string reply)
{
	for (size_t i = 0; i < ops.size(); i++)
	{
		if (send(ops[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
			std::cerr << "send() failed" << std::endl;
	}
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		if (send(sockclient[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
			std::cerr << "send() failed" << std::endl;
	}
}

void	Channel::sendAll2(std::string reply, int fd)
{
	for (size_t i = 0; i < ops.size(); i++)
	{
		if (ops[i].getFduser() != fd)
		{
			if (send(ops[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
				std::cerr << "send() failed" << std::endl;
		}
	}
	for (size_t i = 0; i < sockclient.size(); i++)
	{
		if (sockclient[i].getFduser() != fd)
		{
			if (send(sockclient[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
				std::cerr << "send() failed" << std::endl;
		}
	}
}

void	Channel::firstuserToOp()
{
	ops.push_back(sockclient[0]);
	sockclient.erase(sockclient.begin());
}

bool	Channel::userToOp(std::string& name)
{
	size_t i = 0;
	for(; i < sockclient.size(); i++){
		if(sockclient[i].getNickname() == name)
			break;
	}
	if(i < sockclient.size()){
		ops.push_back(sockclient[i]);
		sockclient.erase(i + sockclient.begin());
		return true;
	}
	return false;
}

bool	Channel::opToUser(std::string& name)
{
	size_t i = 0;
	for(; i < ops.size(); i++){
		if(ops[i].getNickname() == name)
			break;
	}
	if(i < ops.size()){
		sockclient.push_back(ops[i]);
		ops.erase(i + ops.begin());
		return true;
	}
	return false;
}

void	Channel::sendMessage(std::string msg, User &author)
{
	std::cout << "channel:: sendMessage" << std::endl;
	(void)msg;
	(void)author;
}
