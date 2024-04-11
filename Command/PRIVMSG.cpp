#include "../Server/Server.hpp"

std::string	Server::getTargetCh(std::string &target)
{
	Channel	*ch = NULL;
	std::string ch_name;

	ch = getChannel(target);
	if (ch != NULL && ch->getChannelName() == target)
		return (ch->getChannelName());
	return (std::string(""));
}

std::string Server::getTargetUs(int fd)
{
	User	*us = NULL;

	us = getClientFduser(fd);
	return (us == NULL ? std::string("") : us->getNickname());
}

int8_t	Server::parsePrivmsg(std::string split_message[3], std::string split_params[3], int fd)
{
	switch (parsePrivmsgSyntax(split_message, split_params))
	{
		case 0:
			break ;
		case 1:
			sendMessage(ERR_NORECIPIENT(split_message[0]),fd);
			return (-1);
		case 2:
			sendMessage(ERR_NOTEXTTOSEND(), fd);
			return (-1);
		case 3:
			sendMessage(ERR_NEEDMOREPARAMS(getClientFduser(fd)->getNickname()), fd);
			return (-1);
		case 4:
			sendMessage(ERR_NOTOPLEVEL(split_params[0]), fd);
			return (-1);
		case 5:
			sendMessage(ERR_WILDTOPLEVEL(split_params[0]), fd);
			return (-1);
		case 6:
			sendMessage(ERR_TOOMANYTARGETS(split_params[0]), fd);
			return (-1);
		default:
			break ;
	}
	std::string	tmp("");
	if (split_params[0][0] == '&' || split_params[0][0] == '#')
	{
		// std::cout << "Parse Channel" << std::endl;
		std::string	tmp_ch_name(&split_params[0][1]);
		tmp = getTargetCh(tmp_ch_name);
		if (tmp.empty() == 1 || getChannel(&split_params[0][1]) == NULL)
		{
			sendMessage(ERR_NOSUCHNICK(split_params[0]), fd);
			return (1);
		}
	}
	else
	{
		// std::cout << "Parse User" << std::endl;
		tmp = getTargetUs(fd);
		if (tmp.empty() == 1 || getClientFduser(fd) == NULL || getClientNickname(split_params[0]) == NULL)
		{
			sendMessage(ERR_NOSUCHNICK(tmp), fd);
			return (1);
		}
	}
	return (0);
}

void	Server::PRIVMSG(std::string &message, int fd)
{
	// std::cout << "> In PRIVMSG <" << std::endl;
	std::string split_message[3] = {std::string(), std::string(), std::string()};
    if (splitMessage(message, split_message))
		return ;
	
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_message[2], split_params) == 1)
		return ;

	/*
	ERR_NORECIPIENT	OK		ERR_NOTEXTTOSEND OK
	ERR_CANNOTSENDTOCHAN OK	ERR_NOTOPLEVEL OK
	ERR_WILDTOPLEVEL OK		ERR_TOOMANYTARGETS OK
	ERR_NOSUCHNICK OK
	RPL_AWAY
	*/
	if (parsePrivmsg(split_message, split_params, fd))
		return ;

	if (split_params[0][0] == '&' || split_params[0][0] == '#')
	{
		// Target is a channel
		std::vector<User>	ch_usrs = getChannel(&split_params[0][1])->getSockClient();
		for (size_t i = 0; i < ch_usrs.size(); i++)
		{
			// std::cout << getClientFduser(fd)->getHostname() << " != " <<  ch_usrs[i].getHostname() << std::endl;
			if (getClientFduser(fd)->getHostname() != ch_usrs[i].getHostname())
			{
				// std::cout << "hostname target == " << ch_usrs[i].getHostname() << std::endl;
				sendMessage(RPL_PRIVMSG_CH(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), getChannel(&split_params[0][1])->getChannelName(), split_params[1]), ch_usrs[i].getFduser());
			}
		}

		ch_usrs = getChannel(&split_params[0][1])->getSockOp();
		for (size_t i = 0; i < ch_usrs.size(); i++)
		{
			// std::cout << getClientFduser(fd)->getHostname() << " != " <<  ch_usrs[i].getHostname() << std::endl;
			if (getClientFduser(fd)->getHostname() != ch_usrs[i].getHostname())
			{
				// std::cout << "hostname target == " << ch_usrs[i].getHostname() << std::endl;
				sendMessage(RPL_PRIVMSG_CH(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), getChannel(&split_params[0][1])->getChannelName(), split_params[1]), ch_usrs[i].getFduser());
			}
		}
	}
	else
	{
		// Target is a user
		// std::cout << getClientFduser(fd)->getHostname() << " != " <<  getClientNickname(split_params[0])->getHostname() << std::endl;
		if (getClientFduser(fd)->getHostname() != getClientNickname(split_params[0])->getHostname())
		{
			if (getClientNickname(split_params[0])->getisAway())
			{
				sendMessage(RPL_AWAY(getClientNickname(split_params[0])->getUser(), split_params[0], getClientNickname(split_params[0])->getAwayMessage()), fd);
				return ;
			}
			else
				sendMessage(RPL_PRIVMSG_US(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), getClientNickname(split_params[0])->getNickname(), split_params[1]), getClientNickname(split_params[0])->getFduser());
		}
		// sendMessage(RPL_PRIVMSG_US(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), getClientFduser(fd)->getNickname(), split_params[1]), fd);
	}
}