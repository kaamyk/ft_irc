#include "Server.hpp"

void	goToNextSpace( std::string s, size_t *i )
{
	while (s[*i] != 0 && s[*i] != ' ')
	{
		++(*i);
	}
}

bool	Server::checkOption( std::string opt )
{
	return (opt.empty() == 1
	|| (opt[0] != '+' && opt[0] != '-')
	|| (opt.length() == 1));
}

bool	Server::checkChannel( std::string ch )
{
	return (ch.empty() == 1
	|| (ch[0] != '#' && ch[0] != '&')
	|| ch.length() == 1);
}

bool	Server::checkPrefix( std::string pref )
{
	return (pref.empty() == 1
	|| pref[0] != ':'
	|| pref.length() == 1);
}

int	Server::splitParams( std::string params, std::string splitParams[3] )
{
	size_t	i = 0;
	size_t	k = 0;
	for (int8_t j = 0; i < params.length() && j < 3; j++)
	{
		k = i;
		i = params.find_first_of(" ", k);
		if (i == std::string::npos
			|| (j >= 1 && params[k] == ':'))
		{
			splitParams[j] = params.substr(k);
			break ;
		}
		else if (i < params.length())
		{
			splitParams[j] = params.substr(k, i - k);
		}
		i += (1 & (params[i] == ' '));
	}
	return (0);
}

// int	Server::parsePass( std::string split_mess[3] )
// {
// 	/*
// 	Example:
// 		PASS secretpasswordhere
// 	*/

// 	if (split_mess[0].empty() == 0)
// 	{
// 		std::cerr << "Error: parsePass(): prefix should be empty." << std::endl;
// 		return (1);
// 	}
// 	if (split_mess[2].empty() == 1)
// 	{
// 		std::cerr << "Error: parsePass(): parameters should not be empty." << std::endl;
// 		return (1);
// 	}
// 	return (0);
// }

int	Server::parseNick( std::string split_mess[3] )
{
	/*
	Example:
		NICK Wiz	; Introducing new nick "Wiz".
		:WiZ NICK Kilroy	; WiZ changed his nickname to Kilroy.
	*/
	if (split_mess[2].empty() == 1)
	{
		std::cerr << "Error: parseNick(): no nickname given." << std::endl;
		return (1);
	}
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_mess[2], split_params) == 1)
	{
		return (1);
	}
	if (split_params[1].empty() == 0)
	{
		std::cerr << "Error: parseJoin(): too much parameters." << std::endl;
		return (1);
	}
	return (0);
}

int	Server::parseUser( std::string split_mess[3] )
{
	(void) split_mess;
	return (0);
}

int	Server::parseJoin( std::string split_mess[3] )
{
	/*
	Examples:
		JOIN #foobar
				; join channel #foobar.
		JOIN &foo fubar
				; join channel &foo using key "fubar".
		JOIN #foo,&bar fubar
				; join channel #foo using key "fubar" and &bar using no key.
		JOIN #foo,#bar fubar,foobar
				; join channel #foo using key "fubar". channel #bar using key "foobar".
		JOIN #foo,#bar
				; join channels #foo and #bar.
		:WiZ JOIN #Twilight_zone
				; JOIN message from WiZ
	*/
	
	if (split_mess[2].empty() == 1)
	{
		std::cerr << "Error: parseJoin(): parameters can't be empty." << std::endl;
		return (1);
	}

	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_mess[2], split_params) == 1)
	{
		return (1);
	}
	if (split_params[2].empty() == 0)
	{
		std::cerr << "Error: parseJoin(): too much parameters." << std::endl;
		return (1);
	}

	if (split_params[0].empty() == 1)	
	{
		std::cerr << "Error: parseJoin(): need more params.(no parameters given)" << std::endl;
		return (1);
	}
	return (0);
}

int8_t Server::parsePrivmsgSyntax( std::string split_mess[3], std::string split_params[3] )
{
	/*
	Examples:
		:Angel PRIVMSG Wiz :Hello are you receiving this message ?
				; Message from Angel to Wiz.
		PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br
				;Message to Angel.
		PRIVMSG jto@tolsun.oulu.fi :Hello !
				; Message to a client on server tolsun.oulu.fi with username of "jto".
		PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
				; Message to everyone on a server which has a name matching *.fi.
		PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
				; Message to all users who come from a host which has a name matching *.edu.
	*/

	if (split_mess[2].empty() == 1)
		return (3);

	if (split_params[0].empty() == 1
	|| split_params[0].find_first_of(",") != std::string::npos)
		return (6);
	// else
	// {
	// 	if ((split_params[0][0] == '#' || split_params[0][0] == '&'))
	// 	{
	// 		// Check if multiple recipients

	// 		// Recipient is a channel
	// 		size_t rk_pt = split_params[0].find_first_of(".", 0);
	// 		if (rk_pt == std::string::npos || rk_pt == split_params[0].length())
	// 		{
	// 			std::cerr << "Error: parsePrivmsg(): receiver parameter syntax error.(must have a '.')" << std::endl;
	// 			return (4);
	// 		}
	// 			if (split_params[0][rk_pt + 1] == '*')
	// 			{
	// 				std::cerr << "Error: parsePrivmsg(): wild card in hostname." << std::endl;
	// 				return (5);
	// 			}
	// 	}
	// }

	if (split_params[1].empty() == 1 || split_params[1][0] != ':')
		return (3);
	else if (split_params[1][0] != ':' || split_params[1].length() == 1)
		return (2);
	
	return (0);
}

int	Server::parseKick( std::string split_mess[3] )
{
	// std::cout << "> In Server::parseKick()" << std::endl;
	/*
	Examples:
	KICK &Melbourne Matthew
			; Kick Matthew from &Melbourne
	KICK #Finnish John :Speaking English
			; Kick John from #Finnish using "Speaking English" as the reason (comment).
	:WiZ KICK #Finnish John
			; KICK message from WiZ to remove John from channel #Finnish
	*/

	if (split_mess[2].empty() == 1)
	{
		std::cerr << "Error: parseKick(): no parameters" << std::endl;
		return (1);
	}
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_mess[2], split_params) == 1)
	{
		return (1);
	}
	
	if (split_params[0].empty() == 1)
	{
		std::cerr << "Error: parseKick(): not enough arguments(no channel name)" << std::endl;
		return (1);
	}
	else if (split_params[1].empty() == 1)
	{
		std::cerr << "Error: parseKick(): not enough arguments(no user name)" << std::endl;
		return (1);
	}
	
	if (split_params[0][0] != '#' && split_params[0][0] != '&')
	{
		std::cerr << "Error: parseKick(): invalid parameters.(miss '#' || '&' for channel)" << std::endl;
		return (1);
	}
	else if (split_params[0][1] == 0)
	{
		std::cerr << "Error: parseKick(): invalid parameters.(channel name == \"#\" or \"&\")" << std::endl;
		return (1);
	}

	/*
		Check if channel exists
		Check if user exists and is in the channel
		Check if user has right to run the command
		Check if user can be kicked from the channel
	*/
	
	return (0);
}

int	Server::parseInvite( std::string split_mess[3] )
{
	/*
	Examples:
		:Angel INVITE Wiz #Dust
				; User Angel inviting WiZ to channel #Dust
		INVITE Wiz #Twilight_Zone
				; Command to invite WiZ to #Twilight_zone
	*/
	(void) split_mess;
	if (split_mess[2].empty() == 1)
	{
		std::cerr << "Error: parseInvite(): not enough arguments." << std::endl;
		return (1);
	}

	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_mess[2], split_params) == 1)
	{
		return (1);
	}
	
	if (split_params[0].empty() == 1)
	{
		std::cerr << "Error: parseInvite(): not enough arguments(no user name)" << std::endl;
		return (1);
	}
	else if (split_params[1].empty() == 1)
	{
		std::cerr << "Error: parseInvite(): not enough arguments(no channel name)" << std::endl;
		return (1);
	}
	else if (split_params[2].empty() == 0)
	{
		std::cerr << "Error: parseInvite(): invalid arguments.(too much arguments)" << std::endl;
		return (1);
	}
	
	if (isalpha(split_params[0][0]) == 0)
	{
		std::cerr << "parseInvite(): invalide parameters.(Invalid nickname)" << std::endl;
		return (1);
	}
	else if (split_params[1][0] != '#' && split_params[1][0] != '&')
	{
		std::cerr << "parseInvite(): invalid parameters.(second param not a channel name)" << std::endl;
		return (1);
	}
	else if (split_params[1].length() < 2)
	{
		std::cerr << "parseInvite(): invalid parameters.(channel name == \"#\" or \"&\")" << std::endl;
		return (1);
	}
	return (0);
}

int	Server::parseTopic( std::string split_mess[3],std::string split_params[3] )
{
	/*
	Examples:
		:Wiz TOPIC #test :New topic
				;User Wiz setting the topic.
		TOPIC #test :another topic
				;set the topic on #test to "another topic".
		TOPIC #test
				; check the topic for #test.
	*/
	if (split_mess[2].empty() == 1 || split_params[0].empty() == 1
		|| split_params[2].empty() == 0 || (split_params[0][0] != '#' && split_params[0][0] != '&')
		|| split_params[0].length() < 2  || split_params[1][0] != ':' || split_params[1].length() < 2)
	{
		std::cerr << "parseTopic(): invalid parameters." << std::endl;
		return (1);
	}
	return (0);
}

int	Server::parseMode( std::string split_mess[3] )
{
	/*
	Examples:
		Channel Mode:
		MODE #Finnish +im
				; Makes #Finnish channel moderated and 'invite-only'.
		MODE #Finnish +o Kilroy
				; Gives 'chanop' privileges to Kilroy on channel #Finnish.
		MODE #Finnish +v Wiz
				; Allow WiZ to speak on #Finnish.
		MODE #Fins -s
				; Removes 'secret' flag from channel #Fins.
		MODE #42 +k oulu
				; Set the channel key to "oulu".
		MODE #eu-opers +l 10
				; Set the limit for the number of users on channel to 10.
		MODE &oulu +b
				; list ban masks set for channel.
		MODE &oulu +b *!*@*
				; prevent all users from joining.
		MODE &oulu +b *!*@*.edu
				; prevent any user from a hostname matching *.edu from joining.

		User Mode:
		:MODE WiZ -w
				; turns reception of WALLOPS messages off for WiZ.
		:Angel MODE Angel +i
				; Message from Angel to make themselves invisible.
		MODE WiZ -o
				; WiZ 'deopping' (removing operator	status).  The plain reverse of this
				command ("MODE WiZ +o") must not be allowed from users since would bypass
				the OPER command.
	*/
	if (split_mess[2].empty() == 1)
	{
		std::cerr << "Error: parseMode(): no parameters" << std::endl;
		return (1);
	}
	std::string split_params[3] = {std::string(), std::string(), std::string()};

	if (splitParams(split_mess[2], split_params) == 1 || split_params[0].empty() == 1)
	{
		return (1);
	}
	if (split_params[1].find_first_not_of("+-opsitnmlbvkws") != std::string::npos)
	{
		std::cerr << "Error: parseMode(): invalid parameters.(option)" << std::endl;
		return (1);
	}
	else if (split_params[1][0] != '+' && split_params[1][0] != '-')
	{
		std::cerr << "Error: parseMode(): invalid parameters.(+|-)" << std::endl;
		return (1);
	}
	else if (split_mess[0].empty() == 0 && split_mess[0][0] != ':')
	{
		std::cerr << "Error: parseMode(): invalid prefix." << std::endl;
		return (1);
	}

	if (split_params[0][0] == '#' || split_params[0][0] == '&')
	{
				//	Channel mode
		/*
			Check if channel name valid
			Check if channel name exists
			Check if channel name is associated with the right symbol (#|&)
		*/
		if (split_params[1].find_first_not_of("+-opsitnmlbvk") != std::string::npos)
		{
			std::cerr << "Error: parseMode(): invalid option.(channel mode wrong option)" << std::endl;
			return (1);
		}

		if (split_params[1].find_first_of("olvk") != std::string::npos)
		{
			//	Checks for MODE #channel [+|-]opt
			if (split_params[2].empty() == 1)
			{
				std::cerr << "Error: parseMode(): invalid argument.(ovkl No arguments)" << std::endl;
				return (1);
			}

		}
		else if (split_params[1].find_first_of("psitnm") != std::string::npos)
		{
			if (split_params[2].empty() == 0)
			{
				std::cerr << "Error: parseMode(): channel mode has wrong options." << std::endl;
				return (1);
			}
		}
	}
	else
	{
		if (split_params[2].find_first_not_of("+-iwso") != std::string::npos)
		{
			if (split_mess[2].empty() == 1 || split_params[0].empty() == 1 || split_params[1].empty() == 1)
			{
				std::cerr << "Error: parseMode(): need more argument.(user mode need argument)" << std::endl;
				return (1);
			}
			else
			{
				if (split_mess[0].compare(":" + split_params[0]) != 0)
				{
					std::cerr << "Error: parseMode(): invalid argument.(:sender != <nickname>)" << std::endl;
					return (1);
				}
			}
		}
	}
	
	return (0);
}

int8_t	Server::parseCommand( std::string command )
{
	/*
	NICK: Used to set or change the user's nickname.
    USER: Used to specify the username, hostname, servername, and realname of a new user.
    JOIN: Used to join a channel or a list of channels.
    PRIVMSG: Used to send a message to a specific user or channel.
    TOPIC: Used to set or query the topic of a channel.
    MODE: Used to set or query channel modes or user modes.
    KICK: Used to remove a user from a channel.
    INVITE: Used to invite a user to a channel.
	*/
	(void) command;

	// std::string cmds[] = {"NICK", "USER", "JOIN", "PRIVMSG", "TOPIC", "MODE", "KICK", "INVITE"};
	char cmds[] = {'N', 'U', 'J', 'P', 'T', 'M', 'K', 'I'};

	for (int8_t i = 0; i < 8; i++)
	{
		if (command[0] == cmds[i])
		{
			return (i);
		}
	}
	return (-1);
}

int	Server::splitMessage( std::string message, std::string split_mess[3] )
{
	std::string	s_tmp;
	size_t	i = 0;

	if (message[i] == ':')
	{
		s_tmp = message.substr(0, message.find_first_of(" "));
		split_mess[0] = s_tmp;
		// Go to next element
		goToNextSpace(message, &i);
		i += (1 & (message[i] == ' '));
		if (message[i] == 0)
			return (1);
	}
	else
	{
		split_mess[0] = std::string();
	}

	s_tmp = message.substr(i, message.find_first_of(" ", i) - i);
	split_mess[1] = s_tmp;
	goToNextSpace(message, &i);
	i += (1 & (message[i] == ' '));
	if (message[i] == 0)
		return (1);
	else
	{
		s_tmp = message.substr(i);
		split_mess[2].append(s_tmp);
	}
	return (0);
}

// extraire des informations telles que le préfixe, la commande et les paramètres sous forme de vecteurs
// ex: message = "USER root root 0 :root" ==> cmd[1] = "USER" ; cmd[2] = "root root 0 :root"
std::vector<std::string>	Server::parseMessage( std::string &message )
{
	std::vector<std::string>	cmd;
    std::string split_mess[3] = {std::string(), std::string(), std::string()}; // [0]: prefix, [1]:command, [2]: params

	splitMessage(message, split_mess);

	for (size_t i = 0; i < 3; i++)
	{
		cmd.push_back(split_mess[i]);
	}
	return (cmd);
}

// séparer les paramètres de la commande
// ex: "USER root root 0 :root" ==> param[0] = "USER" ; param[1] = "root" ;  param[2] = "root" ; param[3] = "0" param[4]= ":root"
std::vector<std::string> Server::splitParam(std::string& message)
{
    std::vector<std::string>	param;
    std::stringstream	ss(message);
    std::string	line;
    
    while (std::getline(ss, line, ' '))
    {
        param.push_back(line);
    }
    return param;
}

// separer le buffer en lignes individuelles de commandes et stocker chaque ligne dans le vecteur cmd
std::vector<std::string> Server::splitBuffer(std::string buffer)
{
    std::vector<std::string> cmd;
    std::istringstream stm(buffer); // crée un string stream à partir du buffer pour lire avec getLine
    std::string line;
    size_t end;

    while (std::getline(stm, line))
    {
        end = line.find_first_of("\r\n");
        if (end != std::string::npos)
            line.erase(end);
        // std::cout << "line: " << line << std::endl;
        cmd.push_back(line);
    }
    return cmd;
}

// analyser les commandes reçues et extraire le nom de la commande pour diriger l'exécution vers les fonctions appropriées
void	Server::parseCommandList(std::string &message, int fd)
{
	std::vector<std::string>	command;
	std::vector<std::string>	cmd;

	if (message.empty())
		return ;
	command = parseMessage(message);
	cmd = splitParam(message);
	size_t nonspace = message.find_first_not_of(" \t\v");
	if (nonspace != std::string::npos) // enlever les whitespaces au debut
		message = message.substr(nonspace);
	if ((command[1] == "PASS" || command[1] == "pass") && cmd.size())
		PASS(message, fd);
	else if ((command[1] == "NICK" || command[1] == "nick") && cmd.size())
		NICK(message, fd);
	else if ((command[1] == "USER" || command[1] == "user") && cmd.size())
		USER(message, fd);
	else if ((command[1] == "QUIT" || command[1] == "quit") && cmd.size())
		QUIT(message, fd);
	else if ((command[1] == "PING" || command[1] == "ping") && cmd.size())
		PING(message, fd);
	else if ((command[1] == "CAP" || command[1] == "CAP") && cmd.size())
		return ;
	else if (isRegistered(fd) && cmd.size())
	{
		if (command[1] == "JOIN" || command[1] == "join")
			JOIN(message, fd);
		else if (command[1] == "INVITE" || command[1] == "invite")
			INVITE(message, fd);
		else if (command[1] == "WHOIS" || command[1] == "whois")
			WHOIS(message, fd);
		else if (command[1] == "WHO" || command[1] == "who")
			WHO(message, fd);
		else if (command[1] == "TOPIC" || command[1] == "topic")
			TOPIC(message, fd);
		else if (command[1] == "PART" || command[1] == "part")
			PART(message, fd);
		else if (command[1] == "KICK" || command[1] == "kick")
			KICK(message, fd);
		else if (command[1] == "OPER" || command[1] == "oper")
			OPER(message, fd);
		else if (command[1] == "AWAY" || command[1] == "away")
			AWAY(message, fd);
		else if ((command[1] == "MODE" || command[1] == "mode") && cmd[1][0] == '#')
			MODE_CHANNEL(message, fd);
		else if ((command[1] == "MODE" || command[1] == "mode") && cmd[1][0] != '#')
			MODE_USER(message, fd);
		else if (command[1] == "NOTICE" || command[1] == "notice")
			NOTICE(message, fd);
		else if (command[1] == "PRIVMSG" || command[1] == "privmsg")
			PRIVMSG(message, fd);
		else
			sendMessage(ERR_UNKNOWNCOMMAND(getClientFduser(fd)->getNickname(), command[1]),fd);
		// else if (command[1] == "PRIVMSG" || command[1] == "privmsg")
		// {
		// 	SENDMSG(message, fd);
		// }
	}
	else if (!isRegistered(fd))
		sendMessage(ERR_NOTREGISTERED(std::string("*")), fd);
}
