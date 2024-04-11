#include "Bot.hpp"

bool	Bot::sigflag = false;

Bot::Bot(std::string addr, int port, std::string pass, std::string botname): addr(addr), pass(pass), botname(botname)
{
	std::cerr << "---------- BOT CLIENT ----------\n" << std::endl;
	this->connected = false;
	this->pingpong = 0;

	// initiation du socket bot
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		throw std::runtime_error("socket() failed");
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = inet_addr(addr.c_str());
	std::cout << "Connecting to " << addr << ":" << port << std::endl << std::endl;
}

Bot::Bot(const Bot &obj)
{
	*this = obj;
}

Bot	&Bot::operator=(const Bot &obj)
{
	if (this != &obj)
	{
		this->connected = obj.connected;
	}
	return (*this);
}

Bot::~Bot()
{
	sendMessage("QUIT");
	close(sockfd);
}

void	Bot::botStart()
{
	char	buff[1024];
	ssize_t	bytes;
	size_t	nonspace;
	std::string	receive;

	// std::cout << "HERE\n";
	while (connect(sockfd, (struct sockaddr*)&serv, sizeof(serv)) < 0)
	{
		std::cerr << "connect() failed, reconnecting in 5 seconds...\n";
		sleep(5);
	}
	sendMessage("PASS " + pass);
	sendMessage("NICK " + botname);
	sendMessage("USER " + botname + " 0 * bot");
	while (!connected && !Bot::sigflag) // boucle pour l'authentification
	{
		memset(buff, 0, sizeof(buff));
		bytes = recv(sockfd, buff, (sizeof(buff) - 1), 0);
		if (bytes < 0)
		{
			throw (std::runtime_error("recv() failed"));
		}
		else if (bytes == 0)
		{
			std::cout << std::endl;
			throw (std::runtime_error("Bot disconnected"));
		}
		receive = buff;
		nonspace = receive.find_first_of("\n\r");
		if (nonspace != std::string::npos)
			receive = receive.substr(0, nonspace);
		if (receive.find("464") != std::string::npos)
		{
			std::cerr << "Password incorrect!" << std::endl;
			std::cout << std::endl;
			throw (std::runtime_error("Bot disconnected"));
		}
		if (receive.find("433") != std::string::npos)
		{
			std::cerr << "Nickname already in use" << std::endl;
			botname += '_';
			sendMessage("NICK " + botname);
			continue;
		}
		if (receive.find("001") && !connected)
		{
			connected = true;
			std::cout << botname << " connected to ft_irc\n" << std::endl;
		}
	}
	sendMessage("JOIN #" + botname);
	std::cout << "Created #" << botname << " channel\n" << std::endl;
	while (!Bot::sigflag) // boucle pour gerer les commandes
	{
		if (cmd.empty())
			recv_cmd();
		std::string msg = cmd[0];
		cmd.erase(cmd.begin());
		if (msg.empty())
			continue;
		if (msg.find("PING") != std::string::npos)
			sendMessage("PONG ircserv localhost");
		else if ((msg.find("PONG") != std::string::npos) && pingpong > 1)
		{
			pingpong--;
			std::stringstream ss;
			ss << pingpong;
			std::string str = ss.str();
			sendMessage("PING localhost" + str);
		}
		else
		{
			std::vector<std::string>	param = splitMsg(msg); // split la commandes et ses paramètres par des espaces
			// for (size_t i = 0; i < param.size(); i++)
			// {
			// 	std::cout << "param[" << i << "]: " << param[i] << std::endl;
			// }
			if (std::find(param.begin(), param.end(), "JOIN") != param.end())
				std::cout << &param[0].substr(0, param[0].find('!'))[1] << " joined #" << botname << std::endl;
			else if (std::find(param.begin(), param.end(), "PART") != param.end())
				std::cout << &param[0].substr(0, param[0].find('!'))[1] << " left #" << botname << std::endl;
			else if (std::find(param.begin(), param.end(), "PRIVMSG") != param.end()) // si un client join notre channel on l'affiche
			{
				std::string	target = &param[0].substr(0, param[0].find('!'))[1];
				std::string	text;

				for (size_t i = 3; i < param.size(); i++)
				{
					if (i + 1 < param.size())
						text += param[i] + " ";
					else
						text += param[i];
				}
				// std::cout << "target: " << target << std::endl;
				// std::cout << "text: " << text << std::endl;
				if (text.find("pingpong") != std::string::npos || text.find("Pingpong") != std::string::npos)
				{
					pingpong = 42;
					std::stringstream ss;
					ss << pingpong;
					std::string str = ss.str();
					sendMessage("PING localhost" + str);
				}
				else if (param[3] == ":/help")
					sendMessage("NOTICE " + target + " : /flip = flip a coin ; /roll = roll a dice ; /kick = disconnect the bot");
				else if (param[3] == ":/flip")
					sendMessage("NOTICE " + target + " :" + (rand() % 2 == 0 ? "heads" : "tails"));
				else if (param[3] == ":/roll")
					sendMessage("NOTICE " + target + " :" + Bot::toRoll(rand() % 6 + 1));
				else if (param[3] == ":/kick")
				{
					std::cout << std::endl;
					throw (std::runtime_error("Bot disconnected"));
				}
				else if (text.find("pingpong") == std::string::npos && text.find("Pingpong") == std::string::npos && param[3] != ":/help" && param[3] != ":/flip" && param[3] != ":/roll" && param[3] != ":/kick")
				{
					sendMessage("NOTICE " + target + " " + text + " :P");
				}
			}
		}
	}
}

void	Bot::recv_cmd()
{
	char	buff[1024];
	ssize_t	bytes;
	std::string	tmp;

	memset(buff, 0, sizeof(buff));
	bytes = recv(sockfd, buff, (sizeof(buff) - 1), 0);
	if (bytes < 0)
		throw (std::runtime_error("recv() failed"));
	else if (bytes == 0)
	{
		std::cout << std::endl;
		throw (std::runtime_error("Bot disconnected"));
	}
	rec = buff;
	while (rec.find("\r\n") != std::string::npos)
	{
		tmp = rec.substr(0, rec.find("\r\n"));
		rec.erase(0, rec.find("\r\n") + 2);
		cmd.push_back(tmp);
	}
}

std::vector<std::string>	Bot::splitMsg(std::string msg)
{
	std::vector<std::string> vec = std::vector<std::string>();
	size_t	i;

	while ((i = msg.find(" ")) != std::string::npos)
	{
		vec.push_back(msg.substr(0, i));
		msg.erase(0, i + 1);
	}
	vec.push_back(msg);
	return (vec);
}

std::string	Bot::toRoll(size_t n)
{
	std::stringstream	ss;
	ss << n;
	return (ss.str());
}

void	Bot::sendMessage(std::string message)
{
	message += "\r\n";
	if (send(sockfd, message.c_str(), message.size(), 0) < 0)
		throw std::runtime_error("send() failed");
}

void	Bot::signalHandler(int signum)
{
	(void)signum; // evite l'avertissement "unused parameter"
	std::cout << " signal received!\n" << std::endl;
	Bot::sigflag = true; // arreter le serveur
	throw (std::runtime_error("Bot disconnected"));
}
