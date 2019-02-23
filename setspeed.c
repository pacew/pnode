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

void
setspeed (int fd, int bits_per_second)
{
	struct termios2 t;
	
	if (ioctl (fd, TCGETS2, &t) < 0) {
		perror ("TCGETS2");
		exit (1);
	}

	t.c_cflag &= ~CBAUD;
	t.c_cflag |= BOTHER;
	t.c_ispeed = t.c_ospeed = bits_per_second;

	if (ioctl (fd, TCSETS2, &t) < 0) {
		perror ("TCSETS2");
		exit (1);
	}

}
