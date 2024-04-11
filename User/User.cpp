#include "User.hpp"

User::User()
{
    this->fdUser = -1;
    this->nickname = "";
    this->user = "";
    this->ip = "";
    this->buffer = "";
    this->realname = "";
    this->awaymsg = "";
    this->registered = false;
    this->connected = false;
    this->isOp = false;
    this->isInvisible = false;
    this->chanFounder = false;
    this->isAway = false;
    time(&this->created);
    time(&this->idle);
    char	modes[2] = {'i', 'o'};
    for (int i = 0; i < 2; i++)
		modeUser.push_back(std::make_pair(modes[i], false));
}

User::User(int fd, std::string nickname, std::string user): fdUser(fd), nickname(nickname), user(user)
{
    // time(&this->created);
    // time(&this->idle);
}

User::User(User const &obj)
{
    *this = obj;
}

User &User::operator=(User const &obj)
{
    if (this != &obj)
    {
        this->fdUser = obj.fdUser;
        this->nickname = obj.nickname;
        this->user = obj.user;
        this->ip = obj.ip;
        this->buffer = obj.buffer;
        this->registered = obj.registered;
        this->connected = obj.connected;
        this->isOp = obj.isOp;
        this->isInvisible = obj.isInvisible;
        this->modeUser = obj.modeUser;
        this->realname = obj.realname;
        this->chanFounder = obj.chanFounder;
        this->isAway = obj.isAway;
        this->awaymsg = obj.awaymsg;
        this->created = obj.created;
        this->idle = obj.idle;
    }
    return *this;
}

bool User::operator==(User const &rhs)
{
    if (this->nickname == rhs.nickname)
        return (true);
    return (false);
}

User::~User()
{}

int User::getFduser()
{
    return (this->fdUser);
}

std::string &User::getNickname()
{
    return (this->nickname);
}

std::string User::getUser()
{
    return (user.empty() == 1 ? std::string("") : user);
}

std::string User::getIp()
{
    return (ip);
}

std::string User::getBuffer()
{
    return (buffer);
}

std::string User::getHostname()
{
    std::string hostname = getNickname() + "!" + getUser();
    return (hostname);
}

std::string	User::getRealname() const
{
    return (this->realname);
}

std::string User::getModeUser()
{
	std::string	mode;
	for (size_t i = 0; i < modeUser.size(); i++)
	{
		if (modeUser[i].first != 'o' && modeUser[i].second)
			mode.push_back(modeUser[i].first);
	}
	if (!mode.empty())
		mode.insert(mode.begin(), '+');
	return (mode);
}

bool    User::getRegistered()
{
    return (registered);
}

bool    User::getConnected()
{
    return (this->connected);
}

bool    User::getOp()
{
    return (this->isOp);
}

bool    User::getInvisible()
{
    return (this->isInvisible);
}

// vérifie si le user a été invité à rejoindre le canal
bool    User::getInvited(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
            return (true);
    }
    return (false);
}

bool	User::getModeUserOption(size_t i)
{
	return (modeUser[i].second);
}
bool	User::getChanFounder()
{
	return (this->chanFounder);
}

bool    User::getisAway() const
{
    return (this->isAway);
}

const std::string	User::getAwayMessage() const
{
	return (this->awaymsg);
}

time_t	User::getCreated() const
{
    return (created);
}

time_t  User::getIdle() const
{
    return (this->idle);
}

void    User::setFduser(int fd)
{
    this->fdUser = fd;
}

void    User::setNickname(std::string &nickname)
{
    this->nickname = nickname;
}

void    User::setUser(std::string &user)
{
    this->user = user;
}

void    User::setIp(std::string ip)
{
    this->ip = ip;
}

void    User::setBuffer(std::string recv)
{
    buffer += recv;
}

void    User::setRegistered(bool val)
{
    registered = val;
}

void    User::setConnected(bool val)
{
    this->connected = val;
}

void    User::setOp(bool op)
{
    this->isOp = op;
}

void	User::setChanFounder(bool founder)
{
	this->chanFounder = founder;
}

void    User::setInvisible(bool inv)
{
    this->isInvisible = inv;
}

void	User::setModeUser(size_t i, bool mode)
{
	modeUser[i].second = mode;
}

void    User::setRealname(std::string realname)
{
    this->realname = realname;
}

void    User::setIdle()
{
    time(&this->idle);
}

void    User::setAway(bool state, std::string msg)
{
    this->isAway = state;
    this->awaymsg = msg;
}

void    User::removeBuffer()
{
    buffer.clear();
}

void    User::addInvite(std::string &channel)
{
    this->invitation.push_back(channel);
}

// retirer l'invitation quand le user invité rejoint le canal
void    User::removeInvite(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
        {
            this->invitation.erase(this->invitation.begin() + i);
            return ;
        }
    }
}
