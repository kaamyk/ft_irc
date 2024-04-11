#include "../Server/Server.hpp"

void	Server::PART(std::string message, int fd)
{
	std::vector<std::string>	param;
	std::string	reason;
	bool	flag;

	if (!splitPart(message, param, reason, fd))
	{
		sendMessage3(461, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :Not enough parameters\r\n");
		return ;
	}
	for (size_t i = 0; i < param.size(); i++)
	{
		flag = false; // indique si le canal est trouvé
		for (size_t j = 0; j < this->channel.size(); j++)
		{
			if (this->channel[j].getChannelName() == param[i])
			{
				flag = true;
				if (!channel[j].getUserFd(fd) && !channel[j].getOpFd(fd))
				{
					sendMessage2(442, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :You're not on that channel\r\n");
					continue;
				}
				// si on quitte en étant le founder, on reset le privilège
				if (channel[j].getFindUser(getClientFduser(fd)->getNickname())->getChanFounder())
					channel[j].getFindUser(getClientFduser(fd)->getNickname())->setChanFounder(false);
				// flux de chaîne pour construire le message de départ du canal
				std::stringstream	ss;
				ss << ":" << getClientFduser(fd)->getNickname() << "!~" << getClientFduser(fd)->getUser() << "@localhost PART #" << param[i];
				// vérifie si on a une raison de départ
				if (!reason.empty())
					ss << " :" << reason << "\r\n";
				else
					ss << "\r\n";
				channel[j].sendAll(ss.str());
				// supprimer les cannaux en tant qu'opérateur ou membre
				if (channel[j].getOpFd(channel[j].getFindUser(getClientFduser(fd)->getNickname())->getFduser()))
					channel[j].removeOp(channel[j].getFindUser(getClientFduser(fd)->getNickname())->getFduser());
				else
					channel[j].removeUser(channel[j].getFindUser(getClientFduser(fd)->getNickname())->getFduser());
				// si le canal est vide après avoir enlever le client, on supprime le canal
				if (channel[j].numClient() == 0)
					channel.erase(channel.begin() + j);
				else
				{
					if (!channel[j].haveOp())
					{
						channel[j].firstuserToOp();
					}
				}
			}
		}
		if (!flag)
			sendMessage2(403, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :No such channel\r\n");
	}
}

// split la commande PART pour extraire ses paramètres
// (les noms des cannaux et sa raison de départ)
int	Server::splitPart(std::string message, std::vector<std::string> &param, std::string &reason, int fd)
{
	std::string	str;
	std::string	tmp;

	reason = splitPartReason(message, param);
	if (param.size() < 2)
	{
		param.clear();
		return (0);
	}
	param.erase(param.begin()); // supprimer la commande PART de param
	str = param[0]; // associer le nom du cannal à str
	// std::cout << "str: " << str << std::endl;
	param.clear(); // clear le vecteur param pour stocker les noms de cannaux

	// ajouter les noms des cannaux délimités par les virgules
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == ',')
		{
			param.push_back(tmp);
			tmp.clear();
		}
		else
			tmp += str[i];
	}
	param.push_back(tmp); // ajouter le dernier nom de canal

	// si le nom de canal est vide, il est supprimé
	// et l'indice i est décrémenté pour compenser la suppression
	for (size_t i = 0; i < param.size(); i++)
	{
		if (param[i].empty())
			param.erase(param.begin() + i--);
	}

	// sumprimer ':' s'il existe
	if (reason[0] == ':')
		reason.erase(reason.begin());
	else
	{
		// parcourt chaque caractère de la raison jusqu'à trouver le premier espace
		// une fois trouvé, la raison est raccourcie pour contenir uniquement les caractères avant cet espace
		for (size_t i = 0; i < reason.size(); i++)
		{
			if (reason[i] == ' ')
			{
				reason = reason.substr(0, i);
				break;
			}
		}
	}

	for (size_t i = 0; i < param.size(); i++)
	{
		// supprime '#' s'il existe, sinon ce signifier que le nom est mal formaté
		// et on envoie un message d'erreur
		if (*(param[i].begin()) == '#')
			param[i].erase(param[i].begin());
		else
		{
			sendMessage2(403, getClientFduser(fd)->getNickname(), param[i], getClientFduser(fd)->getFduser(), " :No such channel\r\n");
			param.erase(param.begin() + i--);
		}
	}
	return (1);
}

// divise les paramètres en deux (channel et reason) et les stock dans param
// puis on extrait et retourne la raison 
std::string	Server::splitPartReason(std::string &message, std::vector<std::string> &param)
{
	int	count;
	std::stringstream	ss(message);
	std::string	str; 
	std::string	reason;

	count = 2;
	while (ss >> str && count--)
		param.push_back(str);
	if (param.size() != 2)
		return (std::string(""));
	findReason(message, param[1], reason);
	return (reason);
}

// trouver la première occurrence de tofind dans message 
// et stocke tout ce qui se trouve après cette occurrence dans reason
void	Server::findReason(std::string message, std::string tofind, std::string &reason)
{
	size_t	i;
	std::string	str;

	i = 0;
	for (; i < message.size(); i++)
	{
		if (message[i] != ' ')
		{
			for (; i < message.size() && message[i] != ' '; i++)
				str += message[i];
			if (str == tofind)
				break;
			else
				str.clear();
		}
	}
	if (i < message.size())
		reason = message.substr(i);
	i = 0;
	for (; i < reason.size() && reason[i] == ' '; i++)
	reason = reason.substr(i);
}
