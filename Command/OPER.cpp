#include "../Server/Server.hpp"

void	Server::OPER(std::string &message, int fd)
{
	User	*user;
	std::vector<std::string>	param;

	user = getClientFduser(fd);
	param = splitParam(message);
	if (param.size() < 2)
		{sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd); return;}
	if (param[2] != "42") // on suppose que le mot de passe de l'operateur est "42"
		{sendMessage(ERR_PASSWDMISMATCH(std::string("*")), fd); return;}
	user->setOp(true);
	sendMessage(RPL_YOUREOPER(user->getNickname()), fd);
}
