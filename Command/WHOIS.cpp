#include "../Server/Server.hpp"

void	Server::WHOIS(std::string message, int fd)
{
	User	*user; 
	User	*nick; // l'user qu'on veut obtenir les infos
	std::vector<std::string>	param;
	std::string	nickname; // le nickname de l'user qu'in veut obtenir les infos
	std::stringstream create; // le temps de creation de l'user qu'on veut obtenir les infos (signon)

	user = getClientFduser(fd);
	param = splitParam(message);
	if (param.size() < 2)
		{sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd); return;}
	if (param.size() == 2)
		nickname = param[1];
	else
		nickname = param[2];
	nick = getClientNickname(nickname); 
	if (!nick)
	{
		sendMessage(ERR_NOSUCHNICKWHO(user->getNickname(), nickname), fd);
		return ;
	}
	create << nick->getCreated();
	sendMessage(RPL_WHOISUSER(user->getNickname(), nickname, nick->getUser(), nick->getIp(), nick->getRealname()), fd);
	sendMessage(RPL_WHOISIDLE(user->getNickname(), nickname, idle(nick->getIdle()), create.str()), fd);
	if (nick->getOp())
		sendMessage(RPL_WHOISOPERATOR(user->getNickname(), nickname), fd);
	if (nick->getisAway())
		sendMessage(RPL_AWAY(user->getNickname(), nickname, nick->getAwayMessage()), fd);
	sendMessage(RPL_ENDOFWHOIS(user->getNickname(), nickname), fd);
}

// calculer le temps d'inactivité de l'user, la fonction prend le temps de début de l'inactivité
// comme paramètre et retourne une chaîne de caractères représentant la durée écoulée
std::string	Server::idle(time_t start)
{
	time_t times;
	double val;
	std::stringstream ss;
	std::string rawtimes;

	time(&times);
	val = difftime(times, start);

	ss << val;
	rawtimes = ss.str();
	return rawtimes;
}
