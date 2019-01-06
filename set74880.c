/*
 * if 40mhz -> 115200
 * if 26mhz -> 74880
 * important to turn off hardware flow control - 8266 uses those
 * wires for reset and boot mode
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <asm-generic/termbits.h>
#include <asm-generic/ioctls.h>

int
main (int argc, char **argv)
{
	char *dev;
	int fd;
	struct termios2 t;

	dev = "/dev/ttyUSB0";

	if ((fd = open (dev, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		perror ("open");
		exit (1);
	}

	if (ioctl (fd, TCGETS2, &t) < 0) {
		perror ("TCGETS2");
		exit (1);
	}

	t.c_cflag &= ~CBAUD;
	t.c_cflag |= BOTHER;
	t.c_ispeed = t.c_ospeed = 74880;

	if (ioctl (fd, TCSETS2, &t) < 0) {
		perror ("TCSETS2");
		exit (1);
	}

	if (ioctl (fd, TCGETS2, &t) < 0) {
		perror ("TCGETS2 2");
		exit (1);
	}

	printf ("set to %d\n", t.c_ospeed);
	

	return (0);
}
