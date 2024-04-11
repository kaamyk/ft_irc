#include "../Server/Server.hpp"

void	Server::NICK(std::string message, int fd)
{
	// std::cout << "NICK\nfd: " << fd << " ; message: " << message << std::endl;

	User	*user;
	std::string	old;
	
	user = getClientFduser(fd);

	// supprimer les espaces et les ':' au début du message
	std::string::iterator it = message.begin();
    while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v'))
        ++it;
    if (it != message.end() && *it == ':')
        ++it;
    message = std::string(it + 5, message.end()); // supprimer "PASS " du message
	if (message.empty())
		{sendMessage(ERR_NEEDMOREPARAMS(std::string("*")), fd); return;}
	if (usedNickname(message) && user->getNickname() != message) // vérifier si le nickname est déjà utilisé et que ce n'est pas celui de l'user actuel
	{
		if (user->getNickname().empty())
			sendMessage(ERR_NICKNAMEINUSE(std::string(message), std::string(message)), fd);
		else
			sendMessage(ERR_NICKNAMEINUSE(user->getNickname(), std::string(message)), fd);
		return;
	}

	if (!validNickname(message))
	{
		sendMessage(ERR_ERRONEUSNICKNAME(std::string(message)), fd);
		return;
	}

	else
	{
		if (user && user->getRegistered()) // vérifie si l'user est deja enregistré, si oui on change son nickname
		{
			old = user->getNickname();
			user->setNickname(message);
			updateChannel(old, message);
			if (!old.empty() && old != message)
			{
				if (old == "*" && !user->getUser().empty()) // vérifie si l'user est connecté en tant que user non enregistré
				{
					user->setConnected(true);
					sendMessage(RPL_CONNECTED(user->getNickname()), fd);
					sendMessage(RPL_CHANGENICK(user->getNickname(), message), fd);
				}
				else
					sendMessage(RPL_CHANGENICK(old, message), fd);
				return;
			}
		}
		else if (user && !user->getRegistered())
			sendMessage(ERR_NOTREGISTERED(message), fd);
	}
	if (user && user->getRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->getConnected())
	{
		user->setConnected(true);
		sendMessage(RPL_CONNECTED(user->getNickname()), fd);
	}
}

void	Server::updateChannel(std::string old, std::string n_nick)
{
	for(std::vector<Channel>::iterator ch = channel.begin(); ch < channel.end(); ch++)
	{
		// Run trough Users
		for (std::vector<User>::iterator us = (*ch).getSockClient().begin(); us < (*ch).getSockClient().end(); us++)
		{
			if (us->getNickname() == old)
			{
				us->setNickname(n_nick);
				return ;
			}
		}
		// Run trough Operators
		for (std::vector<User>::iterator us = (*ch).getSockOp().begin(); us < (*ch).getSockOp().end(); us++)
		{
			if (us->getNickname() == old)
			{
				us->setNickname(n_nick);
				return ;
			}		
		}
	}
}

bool	Server::usedNickname(std::string &nickname)
{
	for (size_t i = 0; i < this->sockclient.size(); i++)
	{
		if (this->sockclient[i].getNickname() == nickname)
			return (true);
	}
	return (false);
}

bool	Server::validNickname(std::string &nickname)
{
	if (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
		return (false);
	for (size_t i = 1; i < nickname.size(); i++)
	{
		if (!std::isalnum(nickname[i]) && (std::string("-_[]{}\\`|").find(nickname[i]) == std::string::npos))
			return (false);
	}
	return (true);
}
