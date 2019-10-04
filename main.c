#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "config.h"
#include "TabCRC.h"

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

int serve(int fd, FILE *f)
{
	struct sockaddr_in addr;
	char *in_buf;
	unsigned int addrlen = sizeof(addr);
	char *buf = malloc(BUFSZ);
	int n;
	int reltime;
	packet_t pkg;
	// char *addrh;
	// while (1)
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

		n = fread(&pkg, sizeof(packet_t), 1, f);
		reltime = htonl(pkg.RelativeTime);
		while (!feof(f))
		{
			n = sendto(fd, &pkg, sizeof(packet_t), 0, (struct sockaddr *)&addr, addrlen);
			if (n < 0)
			{
				perror("ERROR in sendto");
				exit(1);
			}
			n = fread(&pkg, sizeof(packet_t), 1, f);
			usleep((htonl(pkg.RelativeTime) - reltime) * 1000); //msec -> usec
			reltime = htonl(pkg.RelativeTime);
		}

		// n = sendto(fd, buf, n, 0, (struct sockaddr *)&addr, addrlen);
		// if (n < 0)
		// {
		// 	perror("ERROR in sendto");
		// 	exit(1);
		// }
	}
}

int sanity_check(packet_t *pkg)
{
	if (htonl(pkg->PktSize) != sizeof(packet_t))
	{
		return 1;
	}
	if (memcmp(pkg->SysCode, "sur\0", 4) != 0)
	{
		return 2;
	}
	if (memcmp(pkg->Version, "mf01", 4) != 0)
	{
		return 3;
	}
	if (htons((uint16_t)pkg->Tick) != 1000)
	{
		return 4;
	}
	if (pkg->marker != 0xCDAB)
	{
		return 5;
	}
	if (htons(pkg->Crc) != CalculateCRC((char *)pkg, sizeof(32)))
	{
		return 6;
	}
	if (htons(pkg->MeasurementCrc) != CalculateCRC((char *)&pkg->MeasurementFrame, sizeof(measure_result)))
	{
		return 7;
	}
	return 0;
}

int full_check(FILE *f)
{
	packet_t pkg;
	int data_len;
	int errs[8];
	int count = 0;
	// int reltime;
	// int count = 0;
	rewind(f); // fseek(f, 0, SEEK_SET);
	memset(errs, 0, sizeof(errs));
	while (!feof(f))
	{
		data_len = fread(&pkg, sizeof(packet_t), 1, f);
		errs[sanity_check(&pkg)]++;
		count++;
	}
	if (errs[1])
		printf("%d packets with wrong PktSize\n", errs[1]);
	if (errs[2])
		printf("%d packets with Wrong SysCode\n", errs[2]);
	if (errs[3])
		printf("%d packets with wrong version\n", errs[3]);
	if (errs[4])
		printf("%d packets with tick != 1 ms\n", errs[4]);
	if (errs[5])
		printf("%d packets with marker != 0xABCD\n", errs[5]);
	if (errs[6])
		printf("%d packets with wrong CRC\n", errs[6]);
	if (errs[7])
		printf("%d packets with wrong measurement frame CRC\n", errs[7]);
	printf("%d of %d packets are correct\n", errs[0], count);
	rewind(f);
	return 0;
}

int main(int argc, char **argv)
{
	FILE *f;
	int fd, ret;
	char *fn;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <.bin file>\n", argv[0]);
		exit(1);
	}
	fn = argv[1];
	f = fopen(fn, "rb");
	// printf("%d\n", sizeof(long));

	if ((ret = full_check(f)) != 0)
	{
		fprintf(stderr, "Sanity check failed\n");
		return ret;
	}
	fd = create_server();
	serve(fd, f);

	fclose(f);
	return 0;
}
