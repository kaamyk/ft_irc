#include "Bot.hpp"

bool	isValidArg(std::string port)
{
	return (port.find_first_not_of("0123456789") == std::string::npos && \
	atoi(port.c_str()) >= 1024 && atoi(port.c_str()) <= 65535);
}

int	main(int argc, char **argv)
{
	if (argc != 5)
	{
		std::cerr << "Error: Syntax must be ./botclient <address> <port> <password> <botname>\n";
		return (1);
	}
	if (!isValidArg(argv[2]) || !*argv[3] || std::strlen(argv[3]) > 20 || !*argv[4])
	{
		std::cerr << "Error: Invalid port/password/botname!" << std::endl;
		return (1);
	}
	try
	{
		signal(SIGINT, Bot::signalHandler);
		signal(SIGQUIT, Bot::signalHandler);

		Bot	bot(argv[1], atoi(argv[2]), argv[3], argv[4]);
		bot.botStart();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl << std::endl;
		std::cout << "---------- BOT CLOSED ----------" << std::endl;
		if (std::string(e.what()) == "Bot disconnected")
			return (0);
		return (1);
	}
}
