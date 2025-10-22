/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <arch/io.h>
#include <delay.h>
#include "beep.h"

#define TIMER2_PORT 0x42
#define TIMER_CONTROL_PORT 0x43
#define SPEAKER_PORT 0x61
#define TIMER_FREQ 1193180

/**
 * Fonction pour générer un beep via le PC Speaker
 * @param frequency_hz Fréquence en Hz (ex: 800, 1000, 1500)
 * @param duration_ms Durée en millisecondes
 */
void mainboard_beep(unsigned int frequency_hz, unsigned int duration_ms)
{
	u32 divisor;
	u8 speaker_control;

	if (frequency_hz == 0) {
		printk(BIOS_WARNING, "Beep: Frequency cannot be 0\n");
		return;
	}

	/* Calculer le diviseur pour la fréquence */
	divisor = TIMER_FREQ / frequency_hz;

	/* Configurer le timer 2 en mode square wave */
	outb(0xB6, TIMER_CONTROL_PORT);  /* 10110110b: Timer 2, LSB+MSB, Mode 3 */
	outb(divisor & 0xFF, TIMER2_PORT);        /* LSB */
	outb((divisor >> 8) & 0xFF, TIMER2_PORT); /* MSB */

	/* Activer le speaker (bits 0 et 1) */
	speaker_control = inb(SPEAKER_PORT);
	outb(speaker_control | 0x03, SPEAKER_PORT);

	/* Attendre la durée du beep */
	mdelay(duration_ms);

	/* Désactiver le speaker */
	speaker_control = inb(SPEAKER_PORT);
	outb(speaker_control & ~0x03, SPEAKER_PORT);
}

/**
 * Beeps POST codes standard
 */
void mainboard_post_beep(unsigned int post_code)
{
	switch (post_code) {
	case 0x00: /* Début POST */
		mainboard_beep(1000, 100);
		break;
	
	case 0x10: /* Mémoire initialisée */
		mainboard_beep(800, 50);
		mdelay(50);
		mainboard_beep(800, 50);
		break;
	
	case 0x80: /* Boot ready */
		mainboard_beep(1500, 200);
		break;
	
	case 0xF0: /* Erreur */
		for (int i = 0; i < 3; i++) {
			mainboard_beep(400, 300);
			mdelay(200);
		}
		break;
	
	default:
		/* Pas de beep pour les autres codes */
		break;
	}
}