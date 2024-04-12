#include "Server.hpp"

bool	Server::signal = false;

void    Server::initServer(int port, std::string pass)
{
    struct sockaddr_in addr;

	this->password = pass;
	this->port = port;
	addr.sin_family = AF_INET; // int représentant une famille d'adresse, AF_INET représente une famille IPv4
    addr.sin_addr.s_addr = INADDR_ANY; // structure contenant les adresses IPv4, INADDR_ANY représente n'importe quelles adresses IP
	addr.sin_port = htons(port); // int de 16-bit de l'ordre des octets représentant le port, qui est convertit à l'ordre des octets du réseau avec htons

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // créer un socket pour la communication, SOCK_STREAM précise qu'on veut communiquer en TCP
    if (sockfd < 0)
    {
        std::cerr << "Error: initServer(): socket() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_val, sizeof(opt_val)) < 0)  // configurer les options du socket, SO_REUSEADDR permet au socket de se relier à la même adresse IP et au même port qu'il utilisait précédemment
    {
        std::cerr << "initServer(): sotsockopt() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) // controler les opération sur les fd, l'option O_NONBLOCK permet de retourner immédiatement les opérations meme si elles ne sont pas valables
    {
        std::cerr << "initServer(): fcntl() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (const struct sockaddr*)&addr, sizeof(const struct sockaddr)) < 0) // spécifier sur quelles interfaces réseau et quels ports le serveur doit écouter les connexions entrantes
    {
        std::cerr << "initServer(): bind() failed." << std::endl;
        exit(EXIT_FAILURE);
    }
	if (listen(sockfd, 3) < 0) // donner la permission d'accepter les connexions entrantes sur le socket
	{
		std::cerr << "initServer(): listen() failed." << std::endl;
        exit(EXIT_FAILURE);
	}
	std::cout << "Passive socket: FD[" << sockfd << "]" << std::endl;

	// ici new_client est une structure pollfd qui surveille les fd en vue des événements d'entrée/sortie
	// permettant à un programme d'attendre les événements sur plusieurs fd simultanément 
	// sans avoir recours à des opérations bloquantes
	addr_len = sizeof(addr);
    new_client.fd = sockfd; // précise qu'on veut surveiller le socket fd
    new_client.events = POLLIN; // POLLIN signifie que n'importe quels datas d'évenements peuvent être surveillés
    new_client.revents = 0; // masque de bits indiquant les événements qui se sont produits pour le socket fd
	poll_fd.push_back(new_client); // pousser la structure du socket fd dans poll_fd qui représente un vecteur de structure pollfd 
}

// boucle qui surveille les activités des files descriptors avec poll()
void	Server::checkPoll()
{
	std::cout << "Waiting for a connection...\n";
	while (Server::signal == false)
	{
		// poll attend qu'un événement se produise sur les fd
		status = poll(&poll_fd[0], poll_fd.size(), 5000);
		if (status < 0 && Server::signal == false)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < poll_fd.size(); i++)
		{
			if (poll_fd[i].revents & POLLIN) //verfier si on peut read le socket
			{
				if (poll_fd[i].fd == sockfd)
					Server::acceptClient();
				else
					Server::receiveEvent(poll_fd[i].fd);
			}
		}
	}
	closeFd();
}

// traiter les connexions entrantes sur sockfd créant un nouveau socket dédié à chaque connexion acceptée
void 	Server::acceptClient()
{
	User	client;
	int	cli_sock;
	struct sockaddr_in	client_addr;
 	socklen_t	socklen = sizeof(client_addr);

	cli_sock = accept(sockfd, (sockaddr *)&client_addr, &socklen); // accepte une nouvelle connexion entrante sur sockfd et retourne le fd du nouveau socket créé
    if (cli_sock == -1)
    {
        std::cerr << "Error: Server::acceptClient(): accept() failed." << std::endl;
		exit(EXIT_FAILURE);
    }
	if (fcntl(cli_sock, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Server::acceptClient(): fcntl() failed." << std::endl;
		exit(EXIT_FAILURE);
	}
	new_client.fd = cli_sock;
    new_client.events = POLLIN | POLLOUT;
	new_client.revents = 0;
	client.setFduser(cli_sock);
	client.setIp(inet_ntoa(client_addr.sin_addr));
	// std::cout << "IP: " << client.getIp() << std::endl;
	sockclient.push_back(client);
	poll_fd.push_back(new_client);
	std::cout << "FD[" << cli_sock << "] connected" << std::endl;
}

// lire les données provenant d'un socket et traite les données lues en fonction du socket expéditeur
void	Server::receiveEvent(int fd)
{
	User	*client;
	char	buf[1024]; // array pour stocker les datas recus
	int	bytes;
	std::vector<std::string> command;

	client = getClientFduser(fd);
	memset(buf, 0, sizeof(buf));
	bytes = recv(fd, buf, sizeof(buf) - 1, 0); // recevoir les messages depuis un socket et stock les données correspondantes dans buf
	if (bytes <= 0) // recv retourne -1 si le socket est deconnecté, dans ce cas, on enleve le socket et on supprime les channels
	{
		std::cout << "FD[" << fd << "] disconnected" << std::endl;
		clearChannel(fd);
		removeClientUser(fd);
		removeFd(fd);
		close(fd);
	}
	else
	{
		// std::cout << "receive Signal()" << std::endl;
		client->setBuffer(buf);
		// std::cout << "buf:\n" << buf << std::endl;
		if (client->getBuffer().find_first_of("\r\n") == std::string::npos)
			return ;
		command = splitBuffer(client->getBuffer());
		for (size_t i = 0; i < command.size(); i++)
		{
			std::cout << "<< " << command[i] << std::endl;
			this->parseCommandList(command[i], fd);
		}
		if (getClientFduser(fd)) // supprimer le buffer s'il existe toujours un user
			getClientFduser(fd)->removeBuffer();
	}
}

// recevoir les signaux CtrlC et Ctrl'\'
void	Server::signalHandler(int signum)
{
	(void)signum; // evite l'avertissement "unused parameter"
	std::cout << " signal received!" << std::endl;
	Server::signal = true; // arreter le serveur
}
