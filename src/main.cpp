#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

int init_socket( int &sockfd, struct sockaddr_in *addr )
{
    if (sockfd != 0)
    {
        std::cout << "Error: init_socket(): parameter not initialized." << std::endl;
        return (1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cout << "Error: init_socket(): socket() failed." << std::endl;
        return (1);
    }
    std::cout << "Socket == " << sockfd << std::endl;

	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(PORT);

    int opt_val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) == -1)
    {
        std::cout << "Error: init_socket(): sotsockopt() failed." << std::endl;
        return (1);
    }
    if (fcntl(sockfd, F_SETFD, O_NONBLOCK) == -1)
    {
        std::cout << "Error: init_socket(): fcntl() failed." << std::endl;
        return (1);
    }
    if (bind(sockfd, (const struct sockaddr*) addr, sizeof(const struct sockaddr)) == -1)
    {
        std::cout << "Error: init_socket(): bind() failed." << std::endl;
        return (1);
    }
	if (listen(sockfd, 3) == -1)
	{
		std::cout << "Error: init_socket(): bind() failed." << std::endl;
        return (1);
	}
    return (0);
}

int main( void )
{
    int sockfd = 0;
	struct sockaddr_in addr;

    if (init_socket(sockfd, &addr) == 1 )
    {
        return (1);
    }

    char s[3] = {0};
    while (1)
    {
        if (listen(sock, 3) != -1)
        {
            
        }
        else
        {
            std::cout << "Error: listen() failed." << std::endl;
        }
    }
    
    return (0);
}