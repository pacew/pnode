# pnode - pace's nodemcu stuff

General idea is to leave a terminal program running in one window and
use the utilities in this directory to poke at the board.

Currently assumes serial port is on /dev/ttyUSB0

## Quick setup:

Run "make" to compile the C helper programs.

Run a terminal program and get your NodeMcu board to a ">" prompt.

Run ./bootstrap-putfile to send over the rcvfile() function.  You
don't need to do this again unless you clear out the flash.

Now run ./putfile FILENAME with any other files you want to send over
or update.

You can run ./reset to reset the board.

You can run ./set74880 then push the reset button to see the initial
boot messages at their weird bitrate of 74880 bit/s.  You'll need to
interact with your terminal monitor program to go back to 115200
bit/s.

## Programs:

**reset**

Toggle the DTR and RTS lines to reset the board.

**rcvfile.lua**

A lua program that can quickly copy data from the serial port to a
file in flash.  The file should only contain plain text and the lines
should be less than 255 bytes.  The transmission is terminated by a
line containing only a dot.  Displays a # as each line is processed, then
prints the total byte count received.

**putfile FILENAME**

Copy the give file from the host to the board, first executing
rcvfile.lua.  Display the size of the file in bytes so you can check
what rcvfile prints out.  If the sizes match, there's a pretty good
chance the transmission was successful, but there isn't any real error
checking.  You'll need to run ./bootstrap-putfile once before using putfile.

**bootstrap-putfile**

Creates the file rcvfile.lua by first sending over the function
line-by-line with appropriate delays, then using that function to
receive and store a copy of itself.

**cat.lua**

A program to display the contents of flash files.  First do "putfile
cat.lua", then type "dofile ('cat.lua')" to the ">" prompt.  Use it
like "cat('init.lua')"

**set74880**

Changes the bit rate to 74880 bit/s so you can see the boot messages
that come out right after you press the reset button.  You'll have to
tell your terminal program to go back to the default 115200 eventually.

## Details

The trick to dealing with an interactive command loop on a serial line
is that if you transmit a file to it naively, you eventually overrun
the buffer ... you're sending lines with just a CR or LF terminating
character, but the remote side echos it back with both CR and LF.  So,
./rcvfile.lua is a program to soak up the incoming data without
echoing it.


				