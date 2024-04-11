#include "./Server/Server.hpp"

int	main(int argc, char **argv)
{
	Server	serv;

	if (argc != 3)
	{
		std::cerr << "Error: Syntax must be ./ircserv <port> <password>\n";
		return (1);
	}
	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		if (!serv.isValidArg(argv[1]) || !*argv[2] || strlen(argv[2]) > 20)
		{
			std::cerr << "Error: Invalid port number / password!" << std::endl;
			return (1);
		}
		serv.initServer(atoi(argv[1]), argv[2]);
		serv.checkPoll();
	}
	catch(const std::exception& e)
	{
		serv.closeFd();
		std::cerr << e.what() << std::endl;
		return (0);
	}
	return (0);
}
