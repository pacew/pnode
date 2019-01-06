/* soak up data from multicast all local hosts: 224.0.0.1 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int vflag;

void
usage (void)
{
	fprintf (stderr, "usage: udpmon\n");
	exit (1);
}

void
dump (void *buf, int n)
{
	int i;
	int j;
	int c;

	for (i = 0; i < n; i += 16) {
		printf ("%04x: ", i);
		for (j = 0; j < 16; j++) {
			if (i+j < n)
				printf ("%02x ", ((unsigned char *)buf)[i+j]);
			else
				printf ("   ");
		}
		printf ("  ");
		for (j = 0; j < 16; j++) {
			c = ((unsigned char *)buf)[i+j] & 0x7f;
			if (i+j >= n)
				putchar (' ');
			else if (c < ' ' || c == 0x7f)
				putchar ('.');
			else
				putchar (c);
		}
		printf ("\n");

	}
}

int
main (int argc, char **argv)
{
	int c;
	int sock;
	struct sockaddr_in addr, raddr;
	socklen_t raddrlen;
	int len;
	char buf[10000];
	int port;

	while ((c = getopt (argc, argv, "v")) != EOF) {
		switch (c) {
		case 'v':
			vflag = 1;
			break;
		default:
			usage ();
		}
	}

	if (optind != argc)
		usage ();

	port = 13277;

	sock = socket (AF_INET, SOCK_DGRAM, 0);

	memset (&addr, 0, sizeof addr);
	addr.sin_port = htons (port);
	if (bind (sock, (struct sockaddr *)&addr, sizeof addr) < 0) {
		perror ("bind");
		exit (1);
	}

	printf ("listening on port %d\n", port);

	while (1) {
		raddrlen = sizeof raddr;
		len = recvfrom (sock, buf, sizeof buf - 1, 0,
				(struct sockaddr *)&raddr, &raddrlen);
		if (len < 0) {
			perror ("recvfrom");
			exit (1);
		}
		buf[len] = 0;
		if (vflag) {
			printf ("%s:%d\n", 
				inet_ntoa (raddr.sin_addr),
				ntohs (raddr.sin_port));
			dump (buf, len);
		} else {
			printf ("%s:%d %s\n", 
				inet_ntoa (raddr.sin_addr),
				ntohs (raddr.sin_port),
				buf);
		}
	}

	return (0);
}

