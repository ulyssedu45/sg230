/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_BEEP_H
#define MAINBOARD_BEEP_H

void mainboard_beep(unsigned int frequency_hz, unsigned int duration_ms);
void mainboard_post_beep(unsigned int post_code);

#endif