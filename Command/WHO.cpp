#include "../Server/Server.hpp"

void	Server::WHO(std::string message, int fd)
{
	User	*user;
	User	*tmp;
	std::vector<std::string>	param;

	user = getClientFduser(fd);
	param = splitParam(message);
	if (param.size() < 2)
		{sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd); return;}
	if (param[1][0] == '#')
	{
		Channel *chan = getChannel(param[1]);
		if (chan)
		{
			std::vector<User *> list = chan->getUserList();
			for (size_t i = 0; i < list.size(); i++)
			{
				tmp = list[i];
				if (!tmp->getInvisible())
					sendMessage(RPL_WHOREPLY(user->getNickname(), param[1], tmp->getUser(), tmp->getIp(), " ft_irc ", tmp->getNickname(), whoFlag(tmp, chan), "1", tmp->getRealname()), fd);
			}
		}
		sendMessage(RPL_ENDOFWHO(user->getNickname(), param[1]), fd);
		return ;
	}
	tmp = getClientNickname(param[1]);
	if (tmp)
	{
		if (!tmp->getInvisible())
			sendMessage(RPL_WHOREPLY(user->getNickname(), "*", tmp->getUser(), tmp->getIp(), " ft_irc ", tmp->getNickname(), whoFlag(tmp, NULL), "1", tmp->getRealname()), fd);
	}
	sendMessage(RPL_ENDOFWHO(user->getNickname(), param[1]), fd);
}

// flag pour spécifier le status de l'user dans un channel
std::string	Server::whoFlag(User *user, Channel *chan)
{
	std::string flag;

	if (user->getisAway())
		flag += "G";
	else
		flag += "H";
	if (user->getOp())
		flag += "*";
	if (chan)
	{
		if (user->getChanFounder())
		{
			flag += "~";
			flag += "@";
		}
	}
	return (flag);
}
