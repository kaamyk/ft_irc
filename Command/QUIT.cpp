#include "../Server/Server.hpp"

void	Server::QUIT(std::string message, int fd)
{
	// std::cout << "QUIT\nfd: " << fd << " ; message: " << message << std::endl;

	std::string	reason;
	std::string	reply;

	reason = quitReason(message);
	for (size_t i = 0; i < channel.size(); i++)
	{
		if (channel[i].getUserFd(fd))
		{
			channel[i].removeUser(fd);
			if (channel[i].numClient() == 0) // si le channel est vide apres suppression du user, on supprime le channel
				channel.erase(channel.begin() + i);
			else
			{
				reply = ":" + getClientFduser(fd)->getNickname() + "!~" + getClientFduser(fd)->getUser() + "@localhost QUIT " + reason + "\r\n";
				channel[i].sendAll(reply);
			}
		}
		else if (channel[i].getOpFd(fd))
		{
			channel[i].removeOp(fd);
			if (channel[i].numClient() == 0) // si le channel est vide apres suppression de l'operateur, on supprime le channel
				channel.erase(channel.begin() + i);
			else
			{
				if (!channel[i].haveOp())
				{
					channel[i].firstuserToOp();
				}
				reply = ":" + getClientFduser(fd)->getNickname() + "!~" + getClientFduser(fd)->getUser() + "@localhost QUIT " + reason + "\r\n";
				channel[i].sendAll(reply);
			}
		}
	}
	sendMessage(":ircserv ERROR :Quitting\r\n", fd);
	std::cout << "FD[" << fd << "] disconnected" << std::endl;
	clearChannel(fd);
	removeClientUser(fd);
	removeFd(fd);
	close(fd);
}

// diviser le mot QUIT et le reste de la commande (reason), 
// et la corrige si nécessaire pour s'assurer qu'elle est correctement formattée
std::string	Server::quitReason(std::string message)
{
	std::istringstream stm(message);
	std::string reason;
	std::string	str;

	stm >> str; // lire le premier mot de la commande et le stocker dans str
	// std::cout << "str = " << str << std::endl;
	quitFormatReason(message, str, reason);
	if (reason.empty())
		return (std::string("Quit"));
	if (reason[0] != ':')
	{
		for (size_t i = 0; i < reason.size(); i++)
		{
			if (reason[i] == ' ') 
			{
				reason.erase(i, reason.size() - i);
				break;
			}
		}
		reason.insert(reason.begin(), ':');
	}
	// std::cout << "reason = " << reason << std::endl;
	return reason;
}

// rechercher str dans message, et stocke dans reason
// tout en supprimant les espaces en début de chaîne 
void	Server::quitFormatReason(std::string message, std::string str, std::string &reason)
{
	size_t i = 0;
	for (; i < message.size(); i++){
		if (message[i] != ' ')
		{
			std::string tmp;
			for (; i < message.size() && message[i] != ' '; i++)
				tmp += message[i];
			if (tmp == str)
				break;
			else tmp.clear();
		}
	}
	if (i < message.size())
		reason = message.substr(i);
	i = 0;
	for (; i < reason.size() && reason[i] == ' '; i++);
	reason = reason.substr(i);
}
