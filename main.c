#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"

int create_server()
{
	int fd;
	int tmp;
	struct sockaddr_in addr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}
	tmp = 1; //TRUE
	// setsockopt for multiple connections & rerun immediately after kill
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&tmp, sizeof(int)) < 0)
	{
		perror("ERROR on setsockopt");
		exit(EXIT_FAILURE);
	}

	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORTNO);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("ERROR on binding");
		exit(1);
	}
	printf("Listening on port %d\n", PORTNO);
	// if (listen(fd, MAX_CLIENTS) < 0)
	// {
	// 	perror("ERROR on listen");
	// 	exit(1);
	// }
	return fd;
}

int serve(int fd)
{
	struct sockaddr_in addr;
	char *in_buf;
	unsigned int addrlen = sizeof(addr);
	char *buf = malloc(BUFSZ);
	int n;
	// char *addrh;
	while (1)
	{
		n = recvfrom(fd, buf, BUFSZ, 0, (struct sockaddr *)&addr, &addrlen);
		if (n < 0)
		{
			perror("ERROR in recvfrom");
			exit(1);
		}
		// addrh = inet_ntoa(addr.sin_addr);
		// if (!addrh)
		// {
		// 	perror("ERROR on inet_ntoa");
		// 	exit(1);
		// }
		// printf("rx'd %d bytes from %s\n", n, addrh);
		// addrh = NULL;

		n = sendto(fd, buf, n, 0, (struct sockaddr *)&addr, addrlen);
		if (n < 0)
		{
			perror("ERROR in sendto");
			exit(1);
		}
	}
}

int main(int argc, char **argv)
{
	FILE *f;
	int fd;
	int data_len;
	char* fn = "r.bin";
	// if (argc != 2)
	// {
	// 	fprintf(stderr, "usage: %s <.bin file>\n", argv[0]);
	// 	exit(1);
	// }
	// fn = argv[1];
	f = fopen(fn, "rb");

	fd = create_server();
	serve(fd);
}
