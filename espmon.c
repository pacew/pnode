/*
 * if 40mhz -> 115200
 * if 26mhz -> 74880
 * important to turn off hardware flow control - 8266 uses those
 * wires for reset and boot mode
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/termios.h>

int fd;

int cur_rts;
int cur_dtr;

void
setrts (int val)
{
	int flag;
	flag = TIOCM_RTS;
	if (val)
		ioctl (fd, TIOCMBIS, &flag);
	else
		ioctl (fd, TIOCMBIC, &flag);

	cur_rts = val;
	printf ("[rts=%d]", val);
	fflush (stdout);
}

void
setdtr (int val)
{
	int flag;
	flag = TIOCM_DTR;
	flag = -1;
	if (val)
		ioctl (fd, TIOCMBIS, &flag);
	else
		ioctl (fd, TIOCMBIC, &flag);

	cur_dtr = val;
	printf ("[dtr=%d]", val);
	fflush (stdout);

}

void setspeed(int fd, int bit_per_sec);

int curspeed;

void
speed (int speed)
{
	if (speed != curspeed) {
		printf ("\r\n[speed %d]", speed);
		fflush (stdout);
		curspeed = speed;
		setspeed (fd, curspeed);
	}
}

double
get_secs (void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}

char match[] = "csum 0x4f\r\n";
char *matchp;

int
main (int argc, char **argv)
{
	char *dev;
	struct termios termios, stdin_termios;
	fd_set rset;
	char ch;
	double last, now, delta;
	
	dev = "/dev/ttyUSB0";

	if ((fd = open (dev, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		perror ("open");
		exit (1);
	}

	tcgetattr (fd, &termios);
	cfmakeraw (&termios);
	tcsetattr (fd, TCSANOW, &termios);

	tcgetattr (0, &stdin_termios);
	termios = stdin_termios;
	cfmakeraw (&termios);
	tcsetattr (0, TCSANOW, &termios);

	fcntl (0, F_SETFL, O_NONBLOCK);

	speed (74880);
	setrts (0);
	setdtr (0);

	matchp = match;

	last = get_secs ();
	
	while (1) {
		FD_ZERO (&rset);
		FD_SET (0, &rset);
		FD_SET (fd, &rset);
		if (select (fd + 1, &rset, NULL, NULL, NULL) < 0) {
			perror ("select");
			goto done;
		}

		if (FD_ISSET (0, &rset)) {
			while (read (0, &ch, 1) == 1) {
				ch &= 0x7f;
				switch (ch) {
				case 'C' & 037:
					goto done;
				case 'G' & 037:
					speed(74880);
					break;
				case 'A' & 037:
					speed (115200);
					break;
				case 'I' & 037:
					speed (9600);
					break;
				case 'R' & 037:
					setrts (! cur_rts);
					break;
				case 'D' & 037:
					setdtr (! cur_dtr);
					break;
	
				default:
					write (fd, &ch, 1);
					break;
				}
			}
		}

		if (FD_ISSET (fd, &rset)) {
			now = get_secs ();
			delta = now - last;
			last = now;
			printf ("[%g]", floor (delta * 1e6) / 1000.0);
			while (read (fd, &ch, 1) == 1) {
				if (ch >= ' ' && ch < 0x7f) {
					putchar (ch);
				} else {
					printf ("[%02x]", ch & 0xff);
					if (ch == '\n')
						printf ("\r\n");
				}

				if (matchp) {
					if ((ch & 0x7f) == *matchp) {
						matchp++;
						if (*matchp == 0) {
							speed (115200);
							matchp = NULL;
						}
					} else {
						matchp = match;
					}
				}
			}
			fflush (stdout);
		}
	}

done:
	printf ("\r\n");

	fcntl (0, F_SETFL, 0);
	tcsetattr (0, TCSANOW, &stdin_termios);

	return (0);
}
