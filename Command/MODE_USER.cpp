#include "../Server/Server.hpp"

void	Server::MODE_USER(std::string &message, int fd)
{
	User	*user;
	char	addminus;
	size_t	nonspace;
	std::string	arg;
	std::string	nickname;
	std::string param;
	std::string	modestring;
	std::stringstream	ssmode;
	std::vector<std::string>	paramsplit;

	addminus = '\0';
	arg = ":";
	arg.clear();
	modestring.clear();
	user = getClientFduser(fd);
	nonspace = message.find_first_not_of("MODEmode \t\v");
	if (nonspace != std::string::npos)
		message = message.substr(nonspace);
	else
	{
		sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd);
		return ;
	}
	parseModeUser(message, nickname, modestring, param);
	paramsplit = splitModeUser(param);
	// std::cout << "message: " << message << std::endl;
	// std::cout << "nickname: " << nickname << std::endl;
	// std::cout << "modestring: " << modestring << std::endl;
	// std::cout << "param: " << param << std::endl;
	if (!user)
	{
		sendMessage(ERR_NOSUCHNICK(std::string("*")), fd);
		return ;
	}
	else if (user->getNickname() != nickname)
	{
		sendMessage(ERR_USERSDONTMATCH(user->getNickname()), fd);
		return ;
	}
	else if (modestring.empty())
	{
		sendMessage(RPL_UMODEIS(user->getNickname(), user->getModeUser()), fd);
		return ;
	}
	else if (user)
	{
		for (size_t i = 0; i < modestring.size(); i++)
		{
			if (modestring[i] == '+' || modestring[i] == '-')
				addminus = modestring[i];
			else
			{
				if (modestring[i] == 'i')
					ssmode << modeInvisible(user, addminus, ssmode.str());
				else if (modestring[i] == 'o')
					ssmode << modeOperator(user, addminus, ssmode.str());
				else
					sendMessage(ERR_UMODEUNKNOWNFLAG(user->getNickname()), fd);
			}
		}
	}
	std::string	chain = ssmode.str();
	// std::cout << "CHAIN: " << chain << std::endl;
	if (chain.empty())
		return ;
	sendMessage(RPL_USERMODEI(user->getNickname(), ssmode.str()), fd);
}

void	Server::parseModeUser(std::string message, std::string &nickname, std::string &modestring, std::string &param)
{
	std::istringstream	ss(message);
	size_t	nonspace;

	ss >> nickname;
	ss >> modestring;
	nonspace = message.find_first_not_of(nickname + modestring + " \t\v");
	if (nonspace != std::string::npos)
		param = message.substr(nonspace);
}

std::vector<std::string>	Server::splitModeUser(std::string param)
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

std::string	Server::modeUserAppend(std::string ssmode, char addminus, char mode)
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

std::string	Server::modeInvisible(User *user, char addminus, std::string ssmode) //User *user, 
{
	std::string	str;

	str.clear();
	if (addminus == '+' && !user->getModeUserOption(0))
	{
		user->setModeUser(0, true);
		user->setInvisible(true);
		str = modeUserAppend(ssmode, addminus, 'i');
	}
	else if (addminus == '-' && user->getModeUserOption(0))
	{
		user->setModeUser(0, false);
		user->setInvisible(false);
		str = modeUserAppend(ssmode, addminus, 'i');
	}
	return (str);
}

std::string	Server::modeOperator(User *user, char addminus, std::string ssmode)
{
	std::string	str;

	str.clear();
	if (addminus == '+' && !user->getModeUserOption(1))
	{
		user->setModeUser(1, true);
		user->setOp(true);
		str = modeUserAppend(ssmode, addminus, 'o');
	}
	else if (addminus == '-' && user->getModeUserOption(0))
	{
		user->setModeUser(1, false);
		user->setOp(false);
		str = modeUserAppend(ssmode, addminus, 'o');
	}
	return (str);
}
