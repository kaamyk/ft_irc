#include "../Server/Server.hpp"

void FindPM(std::string cmd, std::string tofind, std::string &str)
{
	size_t i = 0;
	for (; i < cmd.size(); i++){
		if (cmd[i] != ' '){
			std::string tmp;
			for (; i < cmd.size() && cmd[i] != ' '; i++)
				tmp += cmd[i];
			if (tmp == tofind) break;
			else tmp.clear();
		}
	}
	if (i < cmd.size()) str = cmd.substr(i);
	i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	str = str.substr(i);
}

std::string SplitCmdPM(std::string &cmd, std::vector<std::string> &tmp)
{
	std::stringstream ss(cmd);
	std::string str, msg;
	int count = 2;
	while (ss >> str && count--)
		tmp.push_back(str);
	if(tmp.size() != 2) return std::string("");
	FindPM(cmd, tmp[1], msg);
	return msg;
}

std::string SplitCmdPrivmsg(std::string cmd, std::vector<std::string> &tmp)
{
	std::string str = SplitCmdPM(cmd, tmp);
	if (tmp.size() != 2) {tmp.clear(); return std::string("");}
	tmp.erase(tmp.begin());
	std::string str1 = tmp[0]; std::string str2; tmp.clear();
	for (size_t i = 0; i < str1.size(); i++){//split the first string by ',' to get the channels names
		if (str1[i] == ',')
			{tmp.push_back(str2); str2.clear();}
		else str2 += str1[i];
	}
	tmp.push_back(str2);
	for (size_t i = 0; i < tmp.size(); i++)//erase the empty strings
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	if (str[0] == ':') str.erase(str.begin());
	else //shrink to the first space
		{for (size_t i = 0; i < str.size(); i++){if (str[i] == ' '){str = str.substr(0, i);break;}}}
	return  str;
	
}

void	Server::CheckForChannels_Clients(std::vector<std::string> &tmp, int fd)
{
	for(size_t i = 0; i < tmp.size(); i++){
		if (tmp[i][0] == '#'){
			tmp[i].erase(tmp[i].begin());
			if(!getChannel(tmp[i]))//ERR_NOSUCHNICK (401) // if the channel doesn't exist
				{sendMessage3(401, "#" + tmp[i], getClientFduser(fd)->getFduser(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else if (!getChannel(tmp[i])->getFindUser(getClientFduser(fd)->getNickname())) //ERR_CANNOTSENDTOCHAN (404) // if the client is not in the channel
				{sendMessage2(404, getClientFduser(fd)->getNickname(), "#" + tmp[i], getClientFduser(fd)->getFduser(), " :Cannot send to channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
			else tmp[i] = "#" + tmp[i];
		}
		else{
			if (!getClientNickname(tmp[i]))//ERR_NOSUCHNICK (401) // if the client doesn't exist
				{sendMessage3(401, tmp[i], getClientFduser(fd)->getFduser(), " :No such nick/channel\r\n"); tmp.erase(tmp.begin() + i); i--;}
		}
	}
}

void	Server::SENDMSG(std::string message, int fd)
{
    std::vector<std::string> tmp;
    std::string msg = SplitCmdPrivmsg(message, tmp);
    // std::cout << "msg: " << msg << std::endl;
	// for (size_t i = 0; i < tmp.size(); i++)
	// {
	// 	std::cout << "tmp[ " << i << "]: " << tmp[i] << std::endl;
	// }
    if (!tmp.size())//ERR_NORECIPIENT (411) // if the client doesn't specify the recipient
	{
        sendMessage(ERR_NORECIPIENT(getClientFduser(fd)->getNickname()), fd);
        return;
    }
	if (msg.empty())//ERR_NOTEXTTOSEND (412) // if the client doesn't specify the message
		{sendMessage3(412, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :No text to send\r\n"); return;}
	if (tmp.size() > 10) //ERR_TOOMANYTARGETS (407) // if the client send the message to more than 10 clients
		{sendMessage3(407, getClientFduser(fd)->getNickname(), getClientFduser(fd)->getFduser(), " :Too many recipients\r\n"); return;}
	CheckForChannels_Clients(tmp, fd); // check if the channels and clients exist
	for (size_t i = 0; i < tmp.size(); i++){// send the message to the clients and channels
		if (tmp[i][0] == '#'){
			tmp[i].erase(tmp[i].begin());
			std::string resp = ":" + getClientFduser(fd)->getNickname() + "!~" + getClientFduser(fd)->getUser() + "@localhost PRIVMSG #" + tmp[i] + " :" + msg + "\r\n";
			getChannel(tmp[i])->sendAll2(resp, fd);
		}
		else{
			std::string resp = ":" + getClientFduser(fd)->getNickname() + "!~" + getClientFduser(fd)->getUser() + "@localhost PRIVMSG " + tmp[i] + " :" + msg + "\r\n";
			sendMessage(resp, getClientNickname(tmp[i])->getFduser());
		}
	}
}