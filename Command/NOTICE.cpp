#include "../Server/Server.hpp"

void	Server::NOTICE(std::string message, int fd)
{
	User	*user;
	std::string	msg;
	std::vector<std::string>	test;
	std::vector<std::string>	param;
	std::vector<std::string>	div;
	std::vector<std::string>	target;

	user = getClientFduser(fd);
	test = splitParam(message);
	param = parseMessage(message);
	// for(size_t i = 0; i < param.size(); i++)
	// {
	// 	std::cout << "param[" << i << "]: " << param[i] << std::endl;
	// }
	if (user && test.size() < 3)
		return ;
	size_t	pos;
	pos = param[2].find_first_of(' ');
	if (pos != std::string::npos)
	{
		div.push_back(param[2].substr(0, pos));
		div.push_back(param[2].substr(pos + 1));
		// for(size_t i = 0; i < div.size(); i++)
		// {
		// 	std::cout << "div[" << i << "]: " << div[i] << std::endl;
		// }
		msg = div[1];
		target = splitNotice(div[0]);
	}
	else
		return ;
	// std::cout << "msg: " << msg << std::endl;
	// for(size_t i = 0; i < div.size(); i++)
	// {
	// 	std::cout << "div[" << i << "]: " << target[i] << std::endl;
	// }
	std::vector<std::string>::const_iterator it = target.begin();
	std::vector<std::string>::const_iterator ite = target.end();
	for (; it != ite; it++)
	{
		if (it->empty())
			continue;
		if (it->at(0) == '#')
			chanNotice(user, *it, msg);
		else
			userNotice(user, *it, msg);
	}
}

std::vector<std::string>	Server::splitNotice(std::string str)
{
	std::vector<std::string> result;

	size_t	start = 0;
	size_t	end = str.find(",");

	while (end != std::string::npos)
	{
		//changed the value from (start, end) to (start, end - start);
		result.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(",", start);
	}
	result.push_back(str.substr(start, end));
	return (result);
}

void	Server::chanNotice(User *user, std::string target, std::string msg)
{
	Channel	*chan;

	chan = getChannel(target);
	if (!chan)
		return ;
	chan->sendAll(":" + user->getNickname() + "!" + user->getUser() + "@" + user->getIp() + " " + msg + "\r\n");
}

void	Server::userNotice(User *user, std::string target, std::string msg)
{
	int	fd = getFdByNick(target);

	if (fd == -1)
		return;
	sendMessage(":" + user->getNickname() + "!" + user->getUser() + "@" + user->getIp() + " " + msg + "\r\n", fd);
}

