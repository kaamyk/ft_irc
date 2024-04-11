#include "../Server/Server.hpp"

void	Server::MODE_CHANNEL(std::string &message, int fd)
{
	User	*user;
	Channel	*chan;
	char	addminus; // opérateur + ou -
	size_t	nonspace; // position du premier charactère non vide
	std::string channelname; // nom du cannal
	std::string	modestring; // le mode et son opérateur + ou -
	std::stringstream	ssmode; // opérateur + ou -
	std::string param; // argument pour argument pour les modes k, o ou l qu'on va séparé
	std::string	arg; // argument pour les modes k, o ou l qu'on va stocker avant de construire le message
	std::vector<std::string>	paramsplit; // stocké les arguments séparés

	addminus = '\0';
	arg = ":";
	arg.clear();
	modestring.clear();
	user = getClientFduser(fd);

	// effacer les charactères vides pour garder que la commande et ses paramètres
	nonspace = message.find_first_not_of("MODEmode \t\v");
	if (nonspace != std::string::npos)
		message = message.substr(nonspace);
	else
	{
		sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd);
		return ;
	}

	// séparer la commande et ses paramètres dans 
	// message, channelname, modestring et param
	parseModeChan(message, channelname, modestring, param);
	paramsplit = splitModeChan(param);
	// std::cout << "message: " << message << std::endl;
	// std::cout << "channelname: " << channelname << std::endl;
	// std::cout << "modestring: " << modestring << std::endl;
	// std::cout << "param: " << param << std::endl;
	chan = getChannel(channelname.substr(1));
	if (channelname[0] != '#' || !chan) // si le cannal existe
	{
		sendMessage(ERR_NOSUCHCHANNEL(user->getUser(), channelname), fd);
		return ;
	}
	else if (!chan->getUserFd(fd) && !chan->getOpFd(fd)) // si le user/opérateur et dans le cannal
	{
		sendMessage2(442, getClientFduser(fd)->getNickname(), channelname, getClientFduser(fd)->getFduser(), " :You're not on that channel\r\n");
		return ;
	}
	else if (modestring.empty()) // si l'option du mode n'est pas précisé
	{
		sendMessage(RPL_CHANNELMODEIS(user->getNickname(), chan->getChannelName(), chan->getModeChan()) + \
			RPL_CREATIONTIME(user->getNickname(), chan->getChannelName(), chan->getCreatedAt()), fd);
		return ;
	}
	else if (!chan->getOpFd(fd)) // si on est pas opérateur
	{
		sendMessage(ERR_CHANOPRIVSNEEDED(user->getNickname(), chan->getChannelName()), fd);
		return ;
	}
	else if (chan)
	{
		size_t pos = 0;
		for (size_t i = 0; i < modestring.size(); i++)
		{
			if (modestring[i] == '+' || modestring[i] == '-')
				addminus = modestring[i];
			else
			{
				// std::cout << "size: " << paramsplit.size() << std::endl;
				if (modestring[i] == 'i')
					ssmode << modeInviteOnly(chan, addminus, ssmode.str());
				else if (modestring[i] == 't')
					ssmode << modeTopicRest(chan, addminus, ssmode.str());
				else if (modestring[i] == 'k')
					ssmode << modePassword(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
				else if (modestring[i] == 'o')
					ssmode << modeOpPrivi(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
				else if (modestring[i] == 'l')
					ssmode << modeLimit(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
				else
					sendMessage(ERR_UNKNOWNMODE(user->getNickname(), chan->getChannelName(), modestring[i]), fd);
			}
		}
	}
	std::string	chain = ssmode.str();
	// std::cout << "CHAIN: " << chain << std::endl;
	if (chain.empty())
		return ;
	chan->sendAll(RPL_CHANGEMODE(user->getHostname(), chan->getChannelName(), ssmode.str(), arg));
}

// stocker le nom du canal, les modes spécifiés et les paramètres additionnels
void	Server::parseModeChan(std::string message, std::string &channelname, std::string &modestring, std::string &param)
{
	std::istringstream	ss(message);
	size_t	nonspace;

	ss >> channelname;
	ss >> modestring;
	nonspace = message.find_first_not_of(channelname + modestring + " \t\v");
	if (nonspace != std::string::npos)
		param = message.substr(nonspace);
}

// découpe les paramètres extraits avec ',' comme délimiteur
// et stocke ensuite chaque partie découpée dans paramsplit
std::vector<std::string>	Server::splitModeChan(std::string param)
{
	std::vector<std::string>	paramsplit;
	std::string	line;
	std::istringstream	ss(param);

	if (!param.empty() && param[0] == ':')
		param.erase(param.begin());
	while (std::getline(ss, line, ','))
	{
		paramsplit.push_back(line);
		line.clear();
	}
	return (paramsplit);
}

// ssmode = mode déjà présents dans la commande
// addminus = son opérateur + ou - actuel
// mode = mode à ajouter ou à supprimer
// en fonction du + ou - rencontré dans ssmode, 
// elle décide si elle doit inclure addminus dans la chaîne de sortie ou non
std::string	Server::updateAddMinus(std::string ssmode, char addminus, char mode)
{
	std::stringstream ss;

	ss.clear();
	char nul = '\0';
	for (size_t i = 0; i < ssmode.size(); i++)
	{
		if (ssmode[i] == '+' || ssmode[i] == '-')
			nul = ssmode[i];
	}
	if (nul != addminus)
		ss << addminus << mode;
	else
		ss << mode;
	return (ss.str());
}

std::string	Server::modeInviteOnly(Channel *channel, char addminus, std::string ssmode)
{
	std::string	str;

	str.clear();
	if (addminus == '+' && !channel->getModeChanOption(0))
	{
		channel->setModeChan(0, true);
		channel->setOnlyInvited(1);
		str = updateAddMinus(ssmode, addminus, 'i');
	}
	else if (addminus == '-' && channel->getModeChanOption(0))
	{
		channel->setModeChan(0, false);
		channel->setOnlyInvited(0);
		str = updateAddMinus(ssmode, addminus, 'i');
	}
	return (str);
}

std::string	Server::modeTopicRest(Channel *channel, char addminus, std::string ssmode)
{
	std::string	str;

	str.clear();
	if (addminus == '+' && !channel->getModeChanOption(1))
	{
		channel->setModeChan(1, true);
		channel->setTopicRest(true);
		str = updateAddMinus(ssmode, addminus, 't');
	}
	else if (addminus == '-' && channel->getModeChanOption(1))
	{
		channel->setModeChan(1, false);
		channel->setTopicRest(false);
		str = updateAddMinus(ssmode, addminus, 't');
	}
	return (str);
}

std::string	Server::modePassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
	std::string	str;
	std::string	password;

	str.clear();
	password.clear();
	// std::cout << "size: " << paramsplit.size() << std::endl;
	// std::cout << "pos: " << pos << std::endl;
	if (paramsplit.size() > pos)
	{
		password = paramsplit[pos++];
		// std::cout << "password: " << password << std::endl;
	}
	else
	{
		sendMessage(ERR_NEEDMODEPARM(channel->getChannelName(), std::string("(k)")), fd);
		return (str);
	}
	if (!validModePassword(password))
	{
		sendMessage(ERR_NEEDMODEPARM(channel->getChannelName(), std::string("(k)")), fd);
		return (str);
	}
	if (addminus == '+')
	{
		channel->setModeChan(2, true);
		channel->setChannelPass(password);
		if (!arg.empty())
			arg += " ";
		arg += password;
		str = updateAddMinus(ssmode, addminus, 'k');
	}
	else if (addminus == '-' && channel->getModeChanOption(2))
	{
		if (password == channel->getChannelPass())
		{
			channel->setModeChan(2, false);
			channel->setChannelPass("");
			str = updateAddMinus(ssmode, addminus, 'k');
		}
		else
			sendMessage(ERR_KEYSET(channel->getChannelName()), fd);
	}
	return (str);
}

bool	Server::validModePassword(std::string password)
{
	if(password.empty())
		return false;
	for(size_t i = 0; i < password.size(); i++)
	{
		if(!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

std::string	Server::modeOpPrivi(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
	std::string	str;
	std::string	user;

	str.clear();
	user.clear();
	if (paramsplit.size() > pos)
		user = paramsplit[pos++];
	else
	{
		sendMessage(ERR_NEEDMODEPARM(channel->getChannelName(), std::string("(o)")), fd);
		return (str);
	}
	if (!channel->isUserPresent(user))
	{
		sendMessage(ERR_NOSUCHNICKCHAN(channel->getChannelName(), user), fd);
		return (str);
	}
	if (addminus == '+')
	{
		channel->setModeChan(3, true);
		if (channel->userToOp(user))
		{
			str = updateAddMinus(ssmode, addminus, 'o');
			if (!arg.empty())
				arg += " ";
			arg += user;
		}
	}
	else if (addminus == '-')
	{
		channel->setModeChan(3, false);
		if (channel->opToUser(user))
		{
			str = updateAddMinus(ssmode, addminus, 'o');
			if (!arg.empty())
				arg += " ";
			arg += user;
		}
	}
	return (str);
}

std::string	Server::modeLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
	std::string	str;
	std::string	limit;

	str.clear();
	limit.clear();
	if (addminus == '+')
	{
		if (paramsplit.size() > pos)
		{
			limit += paramsplit[pos++];
			if (validModeLimit(limit))
			{
				channel->setModeChan(4, true);
				channel->setLimit(std::atoi(limit.c_str()));
				if (!arg.empty())
					arg += " ";
				arg += limit;
				str = updateAddMinus(ssmode, addminus, 'l');
			}
			else
				sendMessage(ERR_INVALIDMODEPARM(channel->getChannelName(), std::string("(l)")), fd);
		}
		else
			sendMessage(ERR_NEEDMODEPARM(channel->getChannelName(), std::string("(l)")), fd);		
	}
	else if (addminus == '-' && channel->getModeChanOption(4))
	{
		channel->setModeChan(4, false);
		channel->setLimit(0);
		str = updateAddMinus(ssmode, addminus, 'l');
	}
	return (str);
}

bool	Server::validModeLimit(std::string &limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}
