#include "../Server/Server.hpp"

void	Server::USER(std::string &message, int fd)
{
	// std::cout << "USER\nfd: " << fd << " ; message: " << message << std::endl;

	User	*user;
	std::vector<std::string>	param;

	user = getClientFduser(fd);
	param = splitParam(message);
	// for(size_t i = 0; i < param.size(); i++)
	// {
	// 	std::cout << "param[" << i << "]: " << param[i] << std::endl;
	// }
	if (user && param.size() < 5)
		{sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd); return;}
	if (!user || !user->getRegistered())
		sendMessage(ERR_NOTREGISTERED(std::string("*")), fd);
	else if (user && !user->getUser().empty())
		{sendMessage(ERR_ALREADYREGISTERED(user->getNickname()), fd); return;}
	else
	{
		user->setUser(param[1]);
		std::string	realname = param[4];
		realname = realname.substr(1);
		// std::cout << "REAL: " << realname << std::endl;
		user->setRealname(realname);
	}
	if (user && user->getRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->getConnected())
	{
		user->setConnected(true);
		sendMessage(RPL_CONNECTED(user->getNickname()), fd);
	}
}
