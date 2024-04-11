#include "../Server/Server.hpp"

void Server::TOPIC( std::string message, int fd )
{
	std::vector<std::string>	param;
	User	*user = getClientFduser(fd);
	std::string	channame;

	param = splitParam(message);
    std::string split_message[3] = {std::string(), std::string(), std::string()};
    if (splitMessage(message, split_message))
		return ;
	
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_message[2], split_params) == 1)
		return ;
	if (param.size() < 2)
	{
		sendMessage(ERR_NEEDMOREPARAMS(getClientFduser(fd)->getUser()), fd);
		return ;
	}
	channame = param[1].substr(1);
	if (!getChannel(channame))
	{
		sendMessage3(403, "#" + channame, fd, " :No such channel\r\n");
		return;
	}
	if (!(getChannel(channame)->getUserFd(fd)) && !(getChannel(channame)->getOpFd(fd)))
	{
		sendMessage3(442, "#" + channame, fd, " :You're not on that channel\r\n");
		return;
	}
	/*
	Parsing:
		1.	The user must be invited if the channel is invite-only;
		2.	The user's nick/username/hostname must not match any active bans;
		ERR_NEEDMOREPARAMS: user not in a channel and does not specify a #ch 
		RPL_NOTOPIC: No topic is set in the channel
		ERR_CHANOPRIVSNEEDED: 
		ERR_NOTONCHANNEL OK
		RPL_TOPIC OK
	*/
	
	/*
		check the topic
		Check if a topic is set
		return the topic "<channel> :<topic>"
	*/
	Channel	ch = Channel();

	// Check if the channel name exists
	for (size_t i = 0; i < channel.size(); i++)
	{
		if (channel[i].getChannelName() == &(split_params[0][1]))
			ch = channel[i];
	}
	if (ch.getTopicRest() && !ch.isUserOp(user->getNickname()))
	{
		sendMessage(ERR_CHANOPRIVSNEEDED(user->getNickname(), channame), fd);
		return ;
	}
	if (!split_params[0].empty() && split_params[1].empty())
	{
		if (ch.getTopicName().empty() == 1)
			sendMessage(RPL_NOTOPIC(ch.getChannelName()), fd);
		else
		{
			ch.sendAll(RPL_TOPIC(getClientFduser(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
			ch.sendAll(RPL_TOPICWHOTIME(getClientFduser(fd)->getNickname(), ch.getChannelName(), getClientFduser(fd)->getNickname(), ch.getCreatedAt()));
		}
		return ;
	}
	else if (ch.getUserFd(fd) != NULL || ch.getOpFd(fd) != NULL)
		// && (!ch.modeIsActive('t') || (ch.modeIsActive('t') && ch.isOperator(fd))) )
	{
		// Change topic
		// std::cout << "> new topic == " << split_params[1] << std::endl;
		ch.setTopicName(split_params[1].substr(1));
		ch.sendAll(RPL_TOPIC(getClientFduser(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
		ch.sendAll(RPL_TOPICWHOTIME(getClientFduser(fd)->getNickname(), ch.getChannelName(), getClientFduser(fd)->getNickname(), ch.getCreatedAt()));
		return ;
	}
}