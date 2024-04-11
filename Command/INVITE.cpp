#include "../Server/Server.hpp"

void	Server::INVITE(std::string message, int fd)
{
	std::vector<std::string>	param;
	User	*user;
	Channel	*channel;
	std::string	invitedUserName;
	std::string	channelName;

	param = splitParam(message);
	user = getClientFduser(fd);

	// ERR_NEEDMOREPARAMS
	if (param.size() < 2)
		sendMessage(ERR_NEEDMOREPARAMS(user->getNickname()), fd);
	
	// ERR_NOSUCHCHANNEL
	if (!checkChannelExist(param[2]))
	{
		sendMessage2(403, getClientFduser(fd)->getNickname(),  param[2], getClientFduser(fd)->getFduser(), " :No such channel\r\n");
		return;
	}

	invitedUserName = param[1];
	channelName = param[2].substr(1);

	channel = getChannel(channelName);
	//ERR_NOTONCHANNEL
	if (!channel->isUserPresent(user->getNickname()))
	{
		sendMessage2(442, getClientFduser(fd)->getNickname(),  channelName, getClientFduser(fd)->getFduser(), " :You're not on that channel\r\n");
		return;
	}
	//ERR_CHANOPRIVSNEEDED
	else if (channel->getOnlyInvited() == 1 && !channel->isUserOp(user->getNickname()))
	{
		sendMessage2(482, getClientFduser(fd)->getNickname(),  channelName, getClientFduser(fd)->getFduser(), " :You're not channel operator\r\n");
		return;
	}
	//ERR_USERONCHANNEL
	else if (channel->isUserPresent(invitedUserName))
	{
		sendMessage(ERR_USERONCHANNEL(user->getNickname(), invitedUserName, channelName), fd);
		return;
	}
	// RPL_INVITING
	else
	{
		sendMessage(RPL_INVITING(getClientFduser(fd)->getNickname(), invitedUserName, channelName), fd);

		User *invitedUser = getClientNickname(invitedUserName);

		if (invitedUser != NULL)
		{
			invitedUser->addInvite(channelName); // add invite to the user

			//send to the invited user notification
			std::string notif;
			notif = getClientFduser(fd)->getNickname() + " has invited you to the channel #"
					+ channelName + "\r\n";
			sendMessage(notif, invitedUser->getFduser());
		}
	}
}
