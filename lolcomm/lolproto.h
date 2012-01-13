/*
 The LoLComm LED communication library protocol definitions.
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

#ifndef __lolprot_h__
#define __lolprot_h__

#ifdef __cplusplus
extern "C" {
#endif

#define p(x)    Serial.print(x)

/*
 *  LoL Response (return code)
 *  --------------------------
 *
 *  The lolcresponse is sent from Arduino to the host.
 *  Is is used to acknowlegde stateful commands via the
 *  RESULT field.
 */

#define ret_ack     0  // eveything went fine
#define ret_nack    1  // command execution failed
#define ret_wtf     2  // something very strange happened

struct lolret {
    uint8_t cmd;
    uint8_t ret;
} __attribute__ ((packed));


/*
 *  LoL Commands
 *  ------------
 *
 *  LoL commands go from a host system to the Arduino board.
 *  Commands are used to remote-control a LoLShield.
 *
 *  Command size is a byte.
 */

struct coord {
    uint8_t x: 4;
    uint8_t y: 4;
} __attribute__ ((packed));

/*
 *  setall - set all LEDs to a specified brightness.
 */
#define cmd_setall  1
struct cmd_setall_payload {
    uint8_t brightness;
    uint8_t unused;
} __attribute__ ((packed));
/* --- */

/*
 *  setled - set one LED to a specified brightness.
 */
#define cmd_setled  2
struct cmd_setled_payload {
    struct coord coord;
    uint8_t brightness;
} __attribute__ ((packed));
/* --- */

/*
 *  pulse-all - let the whole screen pulse.
 */
#define cmd_pulseall   3
struct cmd_pulseall_payload {
    uint8_t ramp_up_time_ms_pow2;
    uint8_t ramp_up_modifier;

    uint8_t ramp_dn_time_ms_pow2;
    uint8_t ramp_dn_modifier;

    uint8_t lit_time_ms_pow2;
    uint8_t off_time_ms_pow2;
} __attribute__ ((packed));
/* --- */

/*
 *  pulse-led - let a single LED pulse.
 */
#define cmd_pulseled   4
struct cmd_pulseled_payload {
    struct coord coord;

    uint8_t ramp_up_time_ms_pow2;
    uint8_t ramp_up_modifier;

    uint8_t ramp_dn_time_ms_pow2;
    uint8_t ramp_dn_modifier;

    uint8_t lit_time_ms_pow2;
    uint8_t off_time_ms_pow2;
} __attribute__ ((packed));
/* --- */


#define cmd_full_frame 5
    /* 64 bytes of payload; 4 bit brightness value per LED. */
/* --- */

/*
 * Ideas section for more commands:
 * - draw line, brightness, pulse
 * - draw rect, fill, brightness, pulse
 * - draw circle, fill, brightness, pulse
 */


#ifdef __cplusplus
}
#endif

#endif // #ifndef __lolprot_h__

