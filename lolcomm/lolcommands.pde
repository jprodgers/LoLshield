/*
The LoLComm LED communication library command implementations.
Written by Thilo Fromm <kontakt@thilo-fromm.de>.

Writen for the lolcomm app, which was written for the
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

#include <math.h>

#include "Charliplexing.h"
#include "lolproto.h"
#include "lolcommand_funcs.h"

/* lol-level-implementation (bwwee-hee-hee) of lolcomm commands. */

uint8_t handle_setall( struct cmd_setall_payload * pld )
{
    LedSign::Clear(pld->brightness);
    LedSign::Flip(true);
    return ret_ack;
}
/* --- */

uint8_t handle_setled( struct cmd_setled_payload * pld )
{
    LedSign::Set( pld->coord.x, pld->coord.y, pld->brightness );
    LedSign::Flip(true);
    return ret_nack;
}
/* --- */

uint8_t handle_pulseall( struct cmd_pulseall_payload * pld )
{
    unsigned int ramp_up  = pow( pld->ramp_up_time_ms_pow2, 2) ;
    unsigned int ramp_dn  = pow( pld->ramp_dn_time_ms_pow2, 2) ;
    unsigned int lit_time = pow( pld->lit_time_ms_pow2, 2) ;
    unsigned int off_time = pow( pld->off_time_ms_pow2, 2) ;

    /* ramp-up */
    for (int8_t i=0; i <= SHADES; i++) {
        uint8_t sleep =   ramp_up / SHADES 
                        +     ( SHADES / 2 - i ) 
                            * ( ramp_up / (SHADES * pld->ramp_up_modifier ) );
        LedSign::Clear(i);
        LedSign::Flip(true);
        delay( sleep );
    }

    delay( lit_time );

    // exhale
    for (int8_t i=SHADES; i >= 0; i--) {
        uint8_t sleep =   ramp_dn / SHADES 
                        +     ( SHADES / 2 - i ) 
                            * ( ramp_dn / (SHADES * pld->ramp_dn_modifier ) );
        LedSign::Clear(i);
        LedSign::Flip(true);
        delay( sleep );
    }

    // pause
    delay( off_time );

    return ret_ack;
}
/* --- */

uint8_t handle_pulseled( struct cmd_pulseled_payload * pld )
{
    unsigned int ramp_up  = pow( pld->ramp_up_time_ms_pow2, 2) ;
    unsigned int ramp_dn  = pow( pld->ramp_dn_time_ms_pow2, 2) ;
    unsigned int lit_time = pow( pld->lit_time_ms_pow2, 2) ;
    unsigned int off_time = pow( pld->off_time_ms_pow2, 2) ;

    /* ramp-up */
    for (int8_t i=0; i <= SHADES; i++) {
        uint8_t sleep =   ramp_up / SHADES 
                        +     ( SHADES / 2 - i ) 
                            * ( ramp_up / (SHADES * pld->ramp_up_modifier ) );
        LedSign::Set(pld->coord.x, pld->coord.y, i);
        LedSign::Flip(true);
        delay( sleep );
    }

    delay( lit_time );

    // exhale
    for (int8_t i=SHADES; i >= 0; i--) {
        uint8_t sleep =   ramp_dn / SHADES 
                        +     ( SHADES / 2 - i ) 
                            * ( ramp_dn / (SHADES * pld->ramp_dn_modifier ) );
        LedSign::Set(pld->coord.x, pld->coord.y, i);
        LedSign::Flip(true);
        delay( sleep );
    }

    // pause
    delay( off_time );

    return ret_ack;
}
/* --- */

uint8_t handle_full_frame( uint8_t data[64] )
{
    return ret_nack;
}
/* --- */

