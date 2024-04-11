#include "../Server/Server.hpp"

void	Server::AWAY(std::string message, int fd)
{
	User	*user;
	std::vector<std::string>	param;

	user = getClientFduser(fd);
	param = splitParam(message);

	for (size_t i = 0; i < param.size(); i++)
	{
		std::cout << "param[" << i << "]: " << param[i] << std::endl;
	}
	if (param.size() == 1 || param[1].empty())
	{
		user->setAway(false, "");
		sendMessage(RPL_UNAWAY(user->getHostname(), user->getNickname()), fd);
		return ;
	}
	else
	{
		user->setAway(true, param[1]);
		sendMessage(RPL_NOWAWAY(user->getHostname(), user->getNickname()), fd);
		return ;
	}
}
