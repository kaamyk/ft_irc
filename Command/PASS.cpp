#include "../Server/Server.hpp"

void	Server::PASS(std::string message, int fd)
{
	// std::cout << "PASS\nfd: " << fd << " ; message: " << message << std::endl;
	User *user;
    std::string pass;

    user = getClientFduser(fd);
    // supprimer les espaces et les ':' au début du message
    std::string::iterator it = message.begin();
    while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v'))
        ++it;
    if (it != message.end() && *it == ':')
        ++it;
    message = std::string(it + 5, message.end()); // supprimer "PASS " du message

    if (message.empty())
        sendMessage(ERR_NEEDMOREPARAMS(std::string("*")), fd);
    else if (!user->getRegistered()) 
	{
        pass = message;
        if (pass == password)
            user->setRegistered(true);
        else
            sendMessage(ERR_PASSWDMISMATCH(std::string("*")), fd);
    }
    else
        sendMessage(ERR_ALREADYREGISTERED(getClientFduser(fd)->getNickname()), fd);
}
