/* Copyright 2020 Nick Winans
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dissatisfaction65.h"
//#include <math.h>
#include "quantum.h"

#include "analog.h"

// float calc_percent_bat(float voltage) {
//     return 100 * (1.07 + (.01 - 1.07) / pow(1 + pow(voltage / 3.68, 60.66), .34));
// }

void matrix_init_kb(void) {
	// put your keyboard start-up code here
	// runs once when the firmware starts up

	matrix_init_user();
}

void matrix_scan_kb(void) {
	// put your looping keyboard code here
	// runs every cycle (a lot)

    // if (!readPin(F0)) {
    //     send_string(SS_TAP(X_A));
    // }

	matrix_scan_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	// put your per-action keyboard code here
	// runs for every action, just before processing by the firmware

    uprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);

	return process_record_user(keycode, record);
}

void encoder_update_kb(uint8_t index, bool clockwise) {

}

void led_set_kb(uint8_t usb_led) {
	// put your keyboard LED indicator (ex: Caps Lock LED) toggling code here

	led_set_user(usb_led);
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    // Host Keyboard Layer Status

    float measuredvbat = analogRead(9);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    char vbatchar[10];
    dtostrf(1.1, 4, 2, vbatchar); //calc_percent_bat(measuredvbat)

    oled_write(vbatchar, false);
}
#endif
