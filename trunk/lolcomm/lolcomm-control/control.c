/*

Linux LoLShield communication lib command line client.

Written by Thilo Fromm <kontakt@thilo-fromm.de>.

---------------
THIS IS NOT AN ARDUINO APPLICATION. 

It is a remote control tool for Linux hosts
which lets you set LEDs via the command line.

DO NOT TRY TO COMPILE THIS WITH ARDUINO. IT WON'T WORK.
---------------

Writen for the lolcomm example, which was written for the
LoL Shield, designed by Jimmie Rodgers:
http://jimmieprodgers.com/kits/lolshield/

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Version 3 General Public
License as published by the Free Software Foundation; 
or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "lolproto.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"

static int setup_serial( struct termios * oldtio )
{
    int fd;
    struct termios newtio;

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
    if (fd <0) {perror(MODEMDEVICE); exit(-1); }

    tcgetattr(fd,oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = sizeof(struct lolret);   /* blocking read until 5 chars received */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    return fd;
}     
/* --- */

static void close_serial( int fd, struct termios * oldtio )
{
    tcsetattr( fd,TCSANOW,oldtio );
    close( fd );
}
/* --- */

static unsigned int do_setled( int fd, int argc, char ** argv )
{
    uint8_t cmd = cmd_setled;
    struct  cmd_setled_payload setled;

    if (argc != 3) {
        printf("\nInvalid number of arguments for command."
               " Have %d, want %d.\n\n", argc, 3);
        return 1;        
    }

    setled.coord.x    = atoi( argv[0] );
    setled.coord.y    = atoi( argv[1] );
    setled.brightness = atoi( argv[2] );

    write( fd, &cmd,    sizeof( cmd ) );
    write( fd, &setled, sizeof( setled ) );

    return 0;
}
/* --- */

static unsigned int do_setall( int fd, int argc, char ** argv )
{
    uint8_t cmd = cmd_setall;
    struct  cmd_setall_payload setall;

    if (argc != 1) {
        printf("\nInvalid number of arguments for command."
               " Have %d, want %d.\n\n", argc, 1);
        return 1;        
    }

    setall.brightness = atoi( argv[0] );

    write( fd, &cmd,    sizeof( cmd ) );
    write( fd, &setall, sizeof( setall ) );

    return 0;
}
/* --- */

static unsigned int do_pulseall( int fd, int argc, char ** argv )
{
    uint8_t cmd = cmd_pulseall;
    struct  cmd_pulseall_payload pulse;

    if (argc != 6) {
        printf("\nInvalid number of arguments for command."
               " Have %d, want %d.\n\n", argc, 1);
        return 1;        
    }

    pulse.ramp_up_time_ms_pow2 = sqrt( atoi( argv[0] ) );
    pulse.ramp_up_modifier     =       atoi( argv[1] );
    pulse.lit_time_ms_pow2     = sqrt( atoi( argv[2] ) );
    pulse.ramp_dn_time_ms_pow2 = sqrt( atoi( argv[3] ) );
    pulse.ramp_dn_modifier     =       atoi( argv[4] );
    pulse.off_time_ms_pow2     = sqrt( atoi( argv[5] ) );

    write( fd, &cmd,    sizeof( cmd ) );
    write( fd, &pulse,  sizeof( pulse ) );

    return 0;
}
/* --- */

static unsigned int do_pulseled( int fd, int argc, char ** argv )
{
    uint8_t cmd = cmd_pulseled;
    struct  cmd_pulseled_payload pulse;

    if (argc != 8) {
        printf("\nInvalid number of arguments for command."
               " Have %d, want %d.\n\n", argc, 1);
        return 1;        
    }

    pulse.coord.x              =       atoi( argv[0] );
    pulse.coord.y              =       atoi( argv[1] );
    pulse.ramp_up_time_ms_pow2 = sqrt( atoi( argv[2] ) );
    pulse.ramp_up_modifier     =       atoi( argv[3] );
    pulse.lit_time_ms_pow2     = sqrt( atoi( argv[4] ) );
    pulse.ramp_dn_time_ms_pow2 = sqrt( atoi( argv[5] ) );
    pulse.ramp_dn_modifier     =       atoi( argv[6] );
    pulse.off_time_ms_pow2     = sqrt( atoi( argv[7] ) );

    write( fd, &cmd,    sizeof( cmd ) );
    write( fd, &pulse,  sizeof( pulse ) );

    return 0;
}
/* --- */

static void usage(char ** argv)
{
    printf("\nUsage: %s <command> <command specific number of arguments>\n", argv[0]);
    printf("Supported commands:\n\n");
    printf("       setled    Set a single LED. Command syntax:\n");
    printf("                  %s setled <x> <y> <brightness>\n\n", argv[0]);
    printf("       setall    Set all LEDs at once. Command syntax:\n");
    printf("                  %s setall <brightness>\n\n", argv[0]);
    printf("       pulseled  Pulse a single LED. Command syntax:\n");
    printf("                  %s pulseled <x> <y> <ramp-up-time> "
                                 "<ramp-up-modifier> <lit-time> "
                                 "<ramp-dn-time> <ramp-dn-modifier> "
                                 "<off-time> \n", argv[0]);
    printf("                  All time values in ms. The modifier"
                                 " will stretch the first and shorten the"
                                 " last iterations of ram-up.\n\n");
    printf("       pulseall  Pulse all LEDs. Command syntax:\n");
    printf("                  %s pulseall <ramp-up-time> "
                                 "<ramp-up-modifier> <lit-time> "
                                 "<ramp-dn-time> <ramp-dn-modifier> "
                                 "<off-time>\n", argv[0]);
    printf("                  All time values in ms. The modifier"
                                 " will stretch the first and shorten the"
                                 " last iterations of ram-up.\n\n");

}
/* --- */

int main(int argc, char ** argv)
{
    struct termios oldtio;
    int    tty_fd;
    struct lolret ret;
    int err = 0;

    if ( argc < 2 ) {
       usage(argv);
       return 0;
    }

    tty_fd = setup_serial( &oldtio );

    if      ( 0 == strcmp(argv[1], "setled" ) ) 
        err = do_setled(tty_fd, argc - 2, argv + 2);
    else if ( 0 == strcmp(argv[1], "setall" ) ) 
        err = do_setall(tty_fd, argc - 2, argv + 2);
    else if ( 0 == strcmp(argv[1], "pulseall" ) ) 
        err = do_pulseall(tty_fd, argc - 2, argv + 2);
    else if ( 0 == strcmp(argv[1], "pulseled" ) ) 
        err = do_pulseled(tty_fd, argc - 2, argv + 2);
    else    {
        printf("Unknown command %s. Run %s (without arguments) to get help.\n",
                argv[1], argv[0]);
        err=1;
    }

    if (err) {
        printf("Execution of %s failed.\n\n", argv[1]);
        usage(argv);
    } else {
        read( tty_fd, &ret, sizeof( ret ) );
    }

    close_serial( tty_fd, &oldtio );

    return err ? err : ret.ret;
}

