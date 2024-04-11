#include "../Server/Server.hpp"

void	Server::JOIN(std::string message, int fd)
{
	// std::cout << "JOIN\nfd: " << fd << " ; message: " << message << std::endl;

	// param est un vecteur dont chaque indice correspond à une paire de string
	// le premier string représente le nom du canal
	// le deuxième string représente la clé associée si elle est spécifié
	std::vector<std::pair<std::string, std::string> >	param;
	bool	flag;

	if (!splitJoin(param, message, fd))
	{
		sendMessage3(461, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :Not enough parameters\r\n");
		return ;
	}
	if (param.size() > 10)
	{
		sendMessage3(407, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :Too many channels\r\n");
		return ;
	}
	for (size_t i = 0; i < param.size(); i++)
	{
		flag = false;
		for (size_t j = 0; j < this->channel.size(); j++)
		{
			if (this->channel[j].getChannelName() == param[i].first)
			{
				addToExistChannel(param, i, j, fd);
				flag = true;
				break;
			}
		}
		if (!flag)
			addToNewChannel(param, i, fd);
	}
}

// séparer les paramètres de JOIN dans un vecteur de paires de string param
// ex: JOIN <channel1>,<channel2> <key1>,<key2> ==> param[0] = <channel1, key1> ; param[1] = <channel2, key2>
int	Server::splitJoin(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd)
{
	std::vector<std::string>	vec;
	std::istringstream	ss(message);
	std::string	line;
	std::string	channel;
	std::string	key;
	std::string	str;

	// on divise la commandes et ses parametres dans vec
	while (std::getline(ss, line, ' '))
		vec.push_back(line);
	if (vec.size() < 2)
	{
		param.clear();
		return (0);
	}
	vec.erase(vec.begin());
	channel = vec[0];
	vec.erase(vec.begin());
	if (!vec.empty()) // vérifie si les keys sont spécifiées
	{
		key = vec[0];
		vec.clear();
	}
	for (size_t i = 0; i < channel.size(); i++) // extraire les noms de canal
	{
		if (channel[i] == ',')
		{
			param.push_back(std::make_pair(str, "")); // ajoute le nom de canal au vecteur param
			str.clear();
		}
		else
			str += channel[i];
	}
	param.push_back(std::make_pair(str, "")); // ajoute le dernier nom de canal extrait au vecteur param
	
	// si les keys sont spécifiées, il est stocké dans la deuxième partie de chaque paire dans param
	if (!key.empty())
	{
		size_t	j = 0;
		str.clear();
		for (size_t i = 0; i < key.size(); i++)
		{
			if (key[i] == ',') // pour plusieurs keys
			{
				param[j].second = str;
				j++;
				str.clear();
			}
			else
				str += key[i];
		}
		param[j].second = str;
	}
	// supprimer les noms de canal vides
	for (size_t i = 0; i < param.size(); i++)
	{
		if (param[i].first.empty())
			param.erase(param.begin() + i--);
	}
	// vérifie si les noms commencent par un '#' puis l'enlever
	for (size_t i = 0; i < param.size(); i++)
	{
		if (*(param[i].first.begin()) != '#')
		{
			sendMessage2(403, getClientFduser(fd)->getNickname(), param[i].first, getClientFduser(fd)->getFduser(), " :No such channel\r\n");
			param.erase(param.begin() + i--);
		}
		else
			param[i].first.erase(param[i].first.begin());
	}
	return (1);
}

// ajouter un user dans un canal existant
void	Server::addToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i , int j, int fd)
{
	// vérifie si le client est déjà enregistré dans le canal
	if (this->channel[j].getFindUser(getClientFduser(fd)->getNickname()))
		return;
	// vérifie si le nombre maximal de canaux que le user peut rejoindre est atteint
	if (countJoinedChannel(getClientFduser(fd)->getNickname()) >= 10)
	{
		sendMessage3(405, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :You have joined too many channels\r\n");
		return;
	}
	// vérifie si le canal est protégé par un mot de passe et si le mot de passe fourni est incorrect
	if (!this->channel[j].getChannelPass().empty() && this->channel[j].getChannelPass() != param[i].second)
	{
		if (!isInvited(getClientFduser(fd), param[i].first, 0))
		{
			sendMessage2(475, getClientFduser(fd)->getNickname(), "#" + param[i].first, getClientFduser(fd)->getFduser(), " :Cannot join channel (+k) - bad key\r\n");
			return ;
		}
	}
	// vérifie si le canal est uniquement accessible par invitation et si le user est invité
	if (this->channel[j].getOnlyInvited())
	{
		if (!isInvited(getClientFduser(fd), param[i].first, 1))
		{
			sendMessage2(473, getClientFduser(fd)->getNickname(), "#" + param[i].first, getClientFduser(fd)->getFduser(), " :Cannot join channel (+i)\r\n");
			return ;
		}
	}
	// vérifie la capacité maximale des canaux est atteint
	if (this->channel[j].getLimit() && this->channel[j].numClient() >= (size_t)this->channel[j].getLimit())
	{
		sendMessage2(471, getClientFduser(fd)->getNickname(), "#" + param[i].first, getClientFduser(fd)->getFduser(), " :Cannot join channel (+l)\r\n");
		return ;
	}

	// ajoute le client au canal
	User *user = getClientFduser(fd);
	this->channel[j].addMember(*user);
	// envoie les messages de bienvenue au user
	if (channel[j].getTopicName().empty())
		sendMessage(RPL_JOIN(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), param[i].first) + \
			RPL_NAMREPLY(getClientFduser(fd)->getNickname(), channel[j].getChannelName(), channel[j].getChannelList()) + \
			RPL_ENDOFNAMES(getClientFduser(fd)->getNickname(), channel[j].getChannelName()), fd);
	else
		sendMessage(RPL_JOIN(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), param[i].first) + \
			RPL_TOPIC(getClientFduser(fd)->getNickname(), channel[j].getChannelName(), channel[j].getTopicName()) + \
			RPL_NAMREPLY(getClientFduser(fd)->getNickname(), channel[j].getChannelName(), channel[j].getChannelList()) + \
			RPL_ENDOFNAMES(getClientFduser(fd)->getNickname(), channel[j].getChannelName()), fd);
	// envoie un message de bienvenue à tous les membres du canal
	channel[j].sendAll2(RPL_JOIN(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), param[i].first), fd);
}

// ajouter un user dans un nouveau canal qu'on va créer
void	Server::addToNewChannel(std::vector<std::pair<std::string, std::string> >&param, int i, int fd)
{
	User	*user;
	Channel	newChannel;

	user = getClientFduser(fd);
	// vérifie si le nombre maximal de canaux que le user peut rejoindre est atteint
	if (countJoinedChannel(getClientFduser(fd)->getNickname()) >= 10)
	{
		sendMessage3(405, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :You have joined too many channels\r\n");
		return ;
	}
	// crée un nouveau canal avec le nom spécifié par le user
	newChannel.setChannelName(param[i].first);
	user->setChanFounder(true);
	// std::cout << "Channel name: " << newChannel.getChannelName() << std::endl;
	newChannel.addChanOps(*user);
	if (!user->getOp())
		user->setOp(true);
	newChannel.setCreatedAt();
	this->channel.push_back(newChannel);
	// notifie que le user a rejoint le canal
	sendMessage(RPL_JOIN(getClientFduser(fd)->getHostname(), getClientFduser(fd)->getIp(), newChannel.getChannelName()) + \
			RPL_NAMREPLY(getClientFduser(fd)->getNickname(), newChannel.getChannelName(), newChannel.getChannelList()) + \
			RPL_ENDOFNAMES(getClientFduser(fd)->getNickname(), newChannel.getChannelName()), fd);
}

// compter le nombre de canal dont le user est présent
int	Server::countJoinedChannel(std::string user)
{
	int	count = 0;

	for (size_t i = 0; i < this->channel.size(); i++)
	{
		if (this->channel[i].getFindUser(user))
			count++;
	}
	return (count);
}

// vérifie si le user a été invité à rejoindre un canal
bool	Server::isInvited(User *user, std::string channel, int flag)
{
	if (user->getInvited(channel))
	{
		if (flag == 1) // retirer l'invitation de l'user, flag indique si le user a été retiré
			user->removeInvite(channel);
		return (true);
	}
	return (false);
}
