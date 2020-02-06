/*
Copyright 2020 Nick Winans

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include "config_common.h"

#include <stdbool.h>

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x0427
#define DEVICE_VER      0x0001
#define MANUFACTURER    Nicell
#define PRODUCT         Dissatisfaction 65
#define DESCRIPTION     65% Bluetooth keyboard with OLED and Encoder

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define MATRIX_ROW_PINS { D6, B7, B6, D7, C6 }
#define MATRIX_COL_PINS { F7, F6, F4, F5 }
#define MUX_ENABLES { F1, C7 }

#define ENCODERS_PAD_A { D3 }
#define ENCODERS_PAD_B { D2 }

#define ENCODER_SW F0

#define BATTERY_LEVEL_PIN B5

#define ENCODER_RESOLUTION 4

//#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT

#undef I2C_ADDRESS_SA0_1
#define I2C_ADDRESS_SA0_1 0b0111100
#define LCDWIDTH      128
#define LCDHEIGHT     32

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

#endif
