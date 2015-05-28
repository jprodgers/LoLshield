/*
 The LoLComm LED communication library command definitions.
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

#include <inttypes.h>
#include "lolproto.h"

#ifndef __lolcommands_h__
#define __lolcommands_h__

#ifdef __cplusplus
extern "C" {
#endif

uint8_t handle_setall( struct cmd_setall_payload * pld );

uint8_t handle_setled( struct cmd_setled_payload * pld );

uint8_t handle_pulseall( struct cmd_pulseall_payload * pld );

uint8_t handle_pulseled( struct cmd_pulseled_payload * pld );

uint8_t handle_full_frame( uint8_t data[64] );


/*
 * Ideas section for more commands:
 * - draw line
 * - draw rect (line brightness, fill brightness, maybe even pulse line, pulse fill)
 */

#ifdef __cplusplus
}
#endif

#endif // #ifndef __lolcommands_h__
