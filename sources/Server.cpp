#include "classes/Server.hpp"

static bool check(ssize_t result, int erro = -1)
{
	if (result <= erro)
	{
		std::cout << std::strerror(errno) << std::endl;
	}
	return (result <= erro);
}

static int create_socket(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (check(sockfd))
	{
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

static sockaddr_in create_sockaddr()
{
	sockaddr_in _sockaddr;
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = INADDR_ANY;
	_sockaddr.sin_port = htons(PORT);
	return (_sockaddr);
}

static void bind_socket(int &sockfd, sockaddr_in &sockaddr)
{
	if (check(bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))))
	{
		exit(EXIT_FAILURE);
	}
}

static void listen_socket(int &sockfd)
{
	if (check(listen(sockfd, 10)))
	{
		exit(EXIT_FAILURE);
	}
}

static int accept_socket(int &sockfd, sockaddr_in &sockaddr, int &addrlen)
{
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (check(connection))
	{
		exit(EXIT_FAILURE);
	}
	return connection;
}

static void read_from_connection(int &connection, char *buffer)
{
	if (check(read(connection, buffer, BUFFER_SIZE), 0))
	{
		exit(EXIT_FAILURE);
	}
}

static std::string create_response()
{
	return ("HTTP/1.1 200 OK\nContent-Type: image/png\n\n");
}

Server::Server()
{
	int sockfd = create_socket();
	sockaddr_in sockaddr = create_sockaddr();
	bind_socket(sockfd, sockaddr);
	listen_socket(sockfd);
	int addrlen = sizeof(sockaddr);
	while(1)
	{
		int connection = accept_socket(sockfd, sockaddr, addrlen);
		char request[BUFFER_SIZE] = {0};
		read_from_connection(connection, request);
		std::cout << request << std::string(42, '-') << '\n' << std::endl;

		/* #region Brinks */
		std::ifstream file("Dogs.png", std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			std::cout << "Failed to open image file." << std::endl;
			exit(EXIT_FAILURE);
		}
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<char> imageBuffer(size);
		if (!file.read(imageBuffer.data(), size))
		{
			std::cout << "Failed to read image file" << std::endl;
			exit(EXIT_FAILURE);
		}
		file.close();
		/* #endregion */

		std::string response = create_response();
		send(connection, response.c_str(), response.size(), 0);
		send(connection, imageBuffer.data(), imageBuffer.size(), 0);// Parte da brinks
		close(connection);
	}
	close(sockfd);
}

