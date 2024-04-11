#include "../Server/Server.hpp"

void	Server::KICK(std::string message, int fd)
{
	std::vector<std::string>	param;
	std::string	comment;
	std::string	user;
	Channel	*chan;

	comment = splitKick(message, param, user, fd);
	if (user.empty()) // vérifie si le parametre de KICK est vide,
	{
		sendMessage3(461, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :Not enough parameters\r\n");
		return ;
	}
	for (size_t i = 0; i < param.size(); i++) // parcourir les cannaux
	{
		if (getChannel(param[i])) // si un cannal existe, on vérifie s'il y a un user
		{
			chan = getChannel(param[i]);

			// on vérifie si le user recherché est present
			if (!chan->getUserFd(fd) && !chan->getOpFd(fd))
			{
				sendMessage2(442, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :You're not on that channel\r\n");
				continue;
			}
			if (chan->getOpFd(fd)) // on vérifie si le user est un operateur de cannal
			{
				if (chan->getFindUser(user))
				{
					// flux de chaîne pour construire le message de départ du canal
					std::stringstream	ss;
					ss << ":" << getClientFduser(fd)->getNickname() << "!~" << getClientFduser(fd)->getUser() << "@localhost KICK #" << param[i] << " " << user;
					// vérifie si on a un commentaire de KICK
					if (!comment.empty())
						ss << " :" << comment << "\r\n";
					else
						ss << "\r\n";
					chan->sendAll(ss.str());
					// supprimer les cannaux en tant qu'opérateur ou membre
					if (chan->getOpFd(chan->getFindUser(user)->getFduser()))
						chan->removeOp(chan->getFindUser(user)->getFduser());
					else
						chan->removeUser(chan->getFindUser(user)->getFduser());
					// si le canal est vide après avoir enlever le client, on supprime le canal
					if (chan->numClient() == 0)
						channel.erase(channel.begin() + i);
				}
				else
				{
					sendMessage2(441, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :They aren't on that channel\r\n");
					continue;
				}
			}
			else
			{
				sendMessage2(482, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :You're not channel operator\r\n");
				continue;
			}
		}
		else
			sendMessage2(403, getClientFduser(fd)->getNickname(), "#" + param[i], getClientFduser(fd)->getFduser(), " :No such channel\r\n");
	}
}

// split la commande KICK pour extraire ses paramètres
// (les noms des cannaux et son comment)
std::string	Server::splitKick(std::string message, std::vector<std::string> &param, std::string &user, int fd)
{
	std::string	comment;
	std::string	str;
	std::string	tmp;

	comment = splitKickReason(message, param);
	if (param.size() < 3)
		return (std::string(""));
	param.erase(param.begin()); // supprimer la commande KICK de param
	str = param[0]; 
	// std::cout << "str: " << str << std::endl;
	user = param[1];
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
	if (comment[0] == ':')
		comment.erase(comment.begin());
	else
	{
		// parcourt chaque caractère de le comment jusqu'à trouver le premier espace
		// une fois trouvé, le comment est raccourcie pour contenir uniquement les caractères avant cet espace
		for (size_t i = 0; i < comment.size(); i++)
		{
			if (comment[i] == ' ')
			{
				comment = comment.substr(0, i);
				break;
			}
		}
	}

	for (size_t i = 0; i < param.size(); i++)
	{
		// supprime '#' s'il existe, sinon le nom est mal formaté et on envoie un message d'erreur
		if (*(param[i].begin()) == '#')
			param[i].erase(param[i].begin());
		else
		{
			sendMessage2(403, getClientFduser(fd)->getNickname(), param[i], getClientFduser(fd)->getFduser(), " :No such channel\r\n");
			param.erase(param.begin() + i--);
		}
	}
	return (comment);
}

// divise les paramètres en deux (channel et comment) et les stock dans param
// puis on extrait et retourne le comment 
std::string	Server::splitKickReason(std::string &message, std::vector<std::string> &param)
{
	int	count;
	std::stringstream	ss(message);
	std::string	str; 
	std::string	comment;

	count = 3;
	while (ss >> str && count--)
		param.push_back(str);
	if (param.size() != 3)
		return (std::string(""));
	findKickReason(message, param[2], comment);
	return (comment);
}

// trouver la première occurrence de tofind dans message 
// et stocke tout ce qui se trouve après cette occurrence dans comment
void	Server::findKickReason(std::string message, std::string tofind, std::string &comment)
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
		str = message.substr(i);
	i = 0;
	for (; i < comment.size() && comment[i] == ' '; i++);
	comment = comment.substr(i);
}
