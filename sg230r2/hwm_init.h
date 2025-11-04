/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef HWM_INIT_H
#define HWM_INIT_H

/*
 * nct6779d_hwm_init - Initialize NCT6779D hardware monitoring
 *
 * This function initializes the hardware monitoring functionality of the
 * Nuvoton NCT6779D SuperIO chip, including fan control, temperature sensors,
 * and voltage monitoring for the Sophos SG230 R2 mainboard.
 */
void nct6779d_hwm_init(void);

#endif