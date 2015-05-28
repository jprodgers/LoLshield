/*
 The LoLComm LED communication library.
 Written by Thilo Fromm <kontakt@thilo-fromm.de>.
 
 This library enables you to set LEDs on the LoLShield 
 via a serial connection.

 Writen for the LoL Shield, designed by Jimmie Rodgers:
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

#include "Charliplexing.h"
#include "lolproto.h"
#include "lolcommand_funcs.h"

void setup()
{
    LedSign::Init(DOUBLE_BUFFER | GRAYSCALE);
    Serial.begin(115200); // 8N1
}
/* --- */

static void read_bytes(uint8_t * buf, uint8_t len)
{
    uint8_t read = 0;

    while ( read < len ) {

        int ret = Serial.read();
        if ( 0 > ret )
            continue;

        buf[read] = (uint8_t) ret;
        read++;
    }
}
/* --- */

void loop()
{ 
    uint8_t command, oldcmd=0, payload[64]; 
    struct  lolret response;

    read_bytes( &command, sizeof(command) );

    response.cmd = command;

    if (command != oldcmd) {
        LedSign::Clear();
        LedSign::Flip(true);
    }

    switch ( command ) {
        case cmd_setall:
            read_bytes( payload, sizeof( struct cmd_setall_payload ) );
            response.ret = handle_setall( 
                                (struct cmd_setall_payload *) payload);
            break;
        case cmd_setled:
            read_bytes( payload, sizeof( struct cmd_setled_payload ) );
            response.ret = handle_setled(
                                (struct cmd_setled_payload *) payload);
            break;
        case cmd_pulseall:
            read_bytes( payload, sizeof( struct cmd_pulseall_payload ) );
            response.ret = handle_pulseall(
                                (struct cmd_pulseall_payload *) payload);
            break;
        case cmd_pulseled:
            read_bytes( payload, sizeof( struct cmd_pulseled_payload ) );
            response.ret = handle_pulseled(
                                (struct cmd_pulseled_payload *) payload);
            break;
        case cmd_full_frame:
            read_bytes( payload, sizeof( payload ) );
            response.ret = handle_full_frame( payload );
            break;
        default:
            response.ret = ret_wtf;
            break;
    }

    Serial.write((uint8_t *) &response, sizeof(response));
}
/* --- */



