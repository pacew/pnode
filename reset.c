#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void
usage (void)
{
	fprintf (stderr, "usage: reset\n");
	exit (1);
}

int fd;

void
setrts (int val)
{
	int flag;
	flag = TIOCM_RTS;
	if (val)
		ioctl (fd, TIOCMBIS, &flag);
	else
		ioctl (fd, TIOCMBIC, &flag);
}

void
setdtr (int val)
{
	int flag;
	flag = TIOCM_DTR;
	if (val)
		ioctl (fd, TIOCMBIS, &flag);
	else
		ioctl (fd, TIOCMBIC, &flag);
}


int
main (int argc, char **argv)
{
	int c;
	char *dev;
	
	while ((c = getopt (argc, argv, "")) != EOF) {
		switch (c) {
		default:
			usage ();
		}
	}

	if (optind != argc)
		usage ();

	dev = "/dev/ttyUSB0";

	if ((fd = open (dev, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		perror ("open");
		exit (1);
	}

	setdtr (0);
	setrts (0);
	usleep (100 * 1000);
	setdtr (1);
	setrts (1);

	return (0);
}
