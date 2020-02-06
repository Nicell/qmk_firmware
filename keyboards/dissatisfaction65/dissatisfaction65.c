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
#include <math.h>
#include "quantum.h"
#include <string.h>
#include "timer.h"

#ifdef QWIIC_MICRO_OLED_ENABLE
#include "micro_oled.h"
#include "qwiic.h"
#endif

#include "analog.h"

#define BAT_REFRESH 1000
#define OLED_TIMEOUT 300000
#define BAT_DEBOUNCE 5

#define BAT_DISPAY_X 5
#define BAT_DISPLAY_Y 0
#define MATRIX_DISPLAY_X 5
#define MATRIX_DISPLAY_Y 16
#define LOCK_DISPLAY_X 45
#define LOCK_DISPLAY_Y 18
#define MOD_DISPLAY_X 74
#define MOD_DISPLAY_Y 18
#define LAYER_DISPLAY_X 39
#define LAYER_DISPLAY_Y 0
#define ENC_DISPLAY_X 86
#define ENC_DISPLAY_Y 0

bool led_numlock = false;
bool led_capslock = false;
bool led_scrolllock = false;

uint16_t bat_time;
uint32_t oled_time;
bool oled_off = false;
float current_bat = 0;
uint8_t bat_count = BAT_DEBOUNCE;
uint8_t enc_mode = 0;
char *enc_str[] = {"VOL", "SCR", "BRT"};
uint8_t layer;
uint16_t enc_keys[2][3] = {
    { KC_AUDIO_VOL_UP, KC_UP, KC_BRIGHTNESS_UP },
    { KC_AUDIO_VOL_DOWN, KC_DOWN, KC_BRIGHTNESS_DOWN }
};

bool send_oled = false;

float calc_percent_bat(float voltage) {
    return floor(100 * (1.07 + (.01 - 1.07) / pow(1 + pow(voltage / 3.68, 60.66), .34)));
}

void matrix_init_kb(void) {
	// put your keyboard start-up code here
	// runs once when the firmware starts up

	matrix_init_user();
}

uint32_t layer_state_set_kb(uint32_t state) {
    state = layer_state_set_user(state);
    layer = biton32(state);
    return state;
}

bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);

    if (res) {
        led_capslock = led_state.caps_lock;
        led_numlock = led_state.num_lock;
        led_scrolllock = led_state.scroll_lock;
    }

    return res;
}

void draw_batt(void) {
    float measuredvbat = analogRead(9);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    float batpct = calc_percent_bat(measuredvbat);

    if (batpct != current_bat) {
        if (bat_count != BAT_DEBOUNCE) {
            bat_count++;
        } else {
            char vbatchar[5] = "";
            dtostrf(batpct, 3, 0, vbatchar);
            strcat(vbatchar, "%");
            int offset = 0;

            if (batpct < 100) {
                offset = 3;
            } else if (batpct < 10) {
                offset = 6;
            }

            draw_rect_filled_soft(BAT_DISPAY_X, BAT_DISPLAY_Y, 27, 11, PIXEL_ON, NORM);
            draw_line_vert(BAT_DISPAY_X + 28, BAT_DISPLAY_Y + 3, 5, PIXEL_ON, NORM);
            draw_string(BAT_DISPAY_X + 2 - offset, BAT_DISPLAY_Y + 2, vbatchar, PIXEL_ON, XOR, 0);

            bat_count = 0;
            current_bat = batpct;
        }
    } else {
        bat_count = 0;
    }
}

void draw_keyboard_layer(void){
    draw_string(LAYER_DISPLAY_X, LAYER_DISPLAY_Y + 2, "LAYER", PIXEL_ON, NORM, 0);

    draw_rect_filled_soft(LAYER_DISPLAY_X + 31, LAYER_DISPLAY_Y, 11, 11, PIXEL_ON, NORM);
    draw_char(LAYER_DISPLAY_X + 34, LAYER_DISPLAY_Y + 2, layer + 0x30, PIXEL_ON, XOR, 0);
}

void draw_enc_mode(void){
    draw_string(ENC_DISPLAY_X, ENC_DISPLAY_Y + 2, "ENC", PIXEL_ON, NORM, 0);

    draw_rect_filled_soft(ENC_DISPLAY_X + 19, ENC_DISPLAY_Y, 23, 11, PIXEL_ON, NORM);
    draw_string(ENC_DISPLAY_X + 22, ENC_DISPLAY_Y + 2, enc_str[enc_mode], PIXEL_ON, XOR, 0);
}

void draw_keyboard_outline(void) {
    draw_rect_soft(MATRIX_DISPLAY_X, MATRIX_DISPLAY_Y, 33, 13, PIXEL_ON, NORM);
    draw_rect_filled_soft(MATRIX_DISPLAY_X, MATRIX_DISPLAY_Y - 2, 8, 3, PIXEL_ON, NORM);
    draw_pixel(MATRIX_DISPLAY_X, MATRIX_DISPLAY_Y, PIXEL_ON, NORM);
    draw_line_hori(MATRIX_DISPLAY_X + 2, MATRIX_DISPLAY_Y - 1, 4, PIXEL_OFF, NORM);
}

void draw_keyboard_matrix(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
            draw_pixel(MATRIX_DISPLAY_X + y*2 + 2, MATRIX_DISPLAY_Y + x*2 + 2,(matrix_get_row(x) & (1 << y)) > 0, NORM);
        }
    }
}

void draw_keyboard_locks(void) {
    if (led_capslock == true) {
        draw_rect_filled_soft(LOCK_DISPLAY_X + 0, LOCK_DISPLAY_Y, 5 + (3 * 6), 11, PIXEL_ON, NORM);
        draw_string(LOCK_DISPLAY_X + 3, LOCK_DISPLAY_Y +2, "CAP", PIXEL_OFF, NORM, 0);
    } else if (led_capslock == false) {
        draw_rect_filled_soft(LOCK_DISPLAY_X + 0, LOCK_DISPLAY_Y, 5 + (3 * 6), 11, PIXEL_OFF, NORM);
        draw_rect_soft(LOCK_DISPLAY_X + 0, LOCK_DISPLAY_Y, 5 + (3 * 6), 11, PIXEL_ON, NORM);
        draw_string(LOCK_DISPLAY_X + 3, LOCK_DISPLAY_Y +2, "CAP", PIXEL_ON, NORM, 0);
    }
}

void draw_keyboard_mods(void) {
    uint8_t mods = get_mods();

    if (mods & MOD_MASK_SHIFT) {
        draw_rect_filled_soft(MOD_DISPLAY_X + 0, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 3, MOD_DISPLAY_Y + 2, "S", PIXEL_OFF, NORM, 0);
    } else {
        draw_rect_filled_soft(MOD_DISPLAY_X + 0, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_OFF, NORM);
        draw_rect_soft(MOD_DISPLAY_X + 0, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 3, MOD_DISPLAY_Y + 2, "S", PIXEL_ON, NORM, 0);
    }
    if (mods & MOD_MASK_CTRL) {
        draw_rect_filled_soft(MOD_DISPLAY_X + 14, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 17, MOD_DISPLAY_Y + 2, "C", PIXEL_OFF, NORM, 0);
    } else {
        draw_rect_filled_soft(MOD_DISPLAY_X + 14, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_OFF, NORM);
        draw_rect_soft(MOD_DISPLAY_X + 14, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 17, MOD_DISPLAY_Y + 2, "C", PIXEL_ON, NORM, 0);
    }
    if (mods & MOD_MASK_ALT) {
        draw_rect_filled_soft(MOD_DISPLAY_X + 28, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 31, MOD_DISPLAY_Y + 2, "A", PIXEL_OFF, NORM, 0);
    } else {
        draw_rect_filled_soft(MOD_DISPLAY_X + 28, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_OFF, NORM);
        draw_rect_soft(MOD_DISPLAY_X + 28, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 31, MOD_DISPLAY_Y + 2, "A", PIXEL_ON, NORM, 0);
    }
    if (mods & MOD_MASK_GUI) {
        draw_rect_filled_soft(MOD_DISPLAY_X + 42, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 45, MOD_DISPLAY_Y + 2, "G", PIXEL_OFF, NORM, 0);
    } else {
        draw_rect_filled_soft(MOD_DISPLAY_X + 42, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_OFF, NORM);
        draw_rect_soft(MOD_DISPLAY_X + 42, MOD_DISPLAY_Y, 5 + (1 * 6), 11, PIXEL_ON, NORM);
        draw_string(MOD_DISPLAY_X + 45, MOD_DISPLAY_Y + 2, "G", PIXEL_ON, NORM, 0);
    }
}

void init_oled(void) {
    clear_buffer();
    current_bat = 0;
    bat_count = BAT_DEBOUNCE;
    bat_time = timer_read();
    oled_time = timer_read32();
    draw_keyboard_outline();
    draw_keyboard_locks();
    draw_keyboard_mods();
    draw_keyboard_layer();
    draw_enc_mode();
    draw_batt();
}

void draw_display(void) {
    oled_time = timer_read32();

    if (oled_off) {
        init_oled();
    }

    draw_keyboard_matrix();
    draw_keyboard_mods();
    draw_keyboard_locks();
    draw_keyboard_layer();
    draw_enc_mode();

    send_buffer();

    if (oled_off) {
        oled_off = false;
    }
}

void matrix_scan_kb(void) {
	// put your looping keyboard code here
	// runs every cycle (a lot)

    if (!oled_off) {
        if (timer_elapsed(bat_time) > BAT_REFRESH) {
            draw_batt();
            send_buffer();
            bat_time = timer_read();
        }

        if (timer_elapsed32(oled_time) > OLED_TIMEOUT) {
            clear_buffer();
            send_buffer();
            oled_off = true;
        }
    }

    if (send_oled) {
        draw_display();
        send_oled = false;
    }

	matrix_scan_user();
}

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
    init_oled();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	// put your per-action keyboard code here
	// runs for every action, just before processing by the firmware

    if (keycode == ENC_PRESS && record->event.pressed) {
        enc_mode = (enc_mode + 1) % 3;
    }

    send_oled = true;

	return process_record_user(keycode, record);
}

void encoder_update_kb(uint8_t index, bool clockwise) {
    uint8_t mods = get_mods();

    if (mods & MOD_MASK_ALT) {
        if (clockwise) {
            tap_code(KC_TAB);
        } else {
            tap_code16(LSFT(KC_TAB));
        }
    } else {
        tap_code(enc_keys[(clockwise + 1)%2][enc_mode]);
    }

    oled_time = timer_read32();
}
