/* SPDX-License-Identifier: GPL-2.0-only */

/* Sophos SG230 Rev 2 - Thermal Configuration */
/* Configuration optimisée pour 60W maximum */

/* ========================================
 * SEUILS THERMIQUES
 * ======================================== */

/* Xeon E3-1245 v5 : Tjmax = 100°C */
#define DPTF_CPU_PASSIVE	90   /* Throttling passif à 90°C */
#define DPTF_CPU_CRITICAL	100  /* Shutdown à 100°C (Tjmax du CPU) */
#define DPTF_CPU_HOT		95   /* Hot threshold à 95°C */

/* Ventilation progressive (SmartFan IV du NCT6779D) */
#define DPTF_CPU_ACTIVE_AC0	70   /* 100% ventilo à 70°C */
#define DPTF_CPU_ACTIVE_AC1	65   /* 80% ventilo à 65°C */
#define DPTF_CPU_ACTIVE_AC2	60   /* 60% ventilo à 60°C */
#define DPTF_CPU_ACTIVE_AC3	50   /* 40% ventilo à 50°C */
#define DPTF_CPU_ACTIVE_AC4	40   /* 30% ventilo à 40°C */

/* ========================================
 * TABLE DE RELATIONS THERMIQUES
 * ======================================== */
Name (DTRT, Package () {
	/* CPU influence sa propre température */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },
})

/* ========================================
 * LIMITES DE PUISSANCE (MPPC)
 * Configuration : 10W idle → 60W max
 * ======================================== */
Name (MPPC, Package ()
{
	0x2,		/* Revision */
	
	/* Power Limit 1 (PL1) - Limite soutenue */
	Package () {
		0,	/* PowerLimitIndex = 0 (PL1) */
		10000,	/* PowerLimitMinimum = 10W (économie en idle) */
		60000,	/* PowerLimitMaximum = 60W (limite max) */
		1000,	/* TimeWindowMinimum = 1s */
		32000,	/* TimeWindowMaximum = 32s */
		500	/* StepSize = 0.5W (ajustements fins) */
	},
	
	/* Power Limit 2 (PL2) - Turbo court terme */
	Package () {
		1,	/* PowerLimitIndex = 1 (PL2) */
		15000,	/* PowerLimitMinimum = 15W */
		65000,	/* PowerLimitMaximum = 65W (turbo limité) */
		0,	/* TimeWindowMinimum = 0 (instant) */
		2000,	/* TimeWindowMaximum = 2s (turbo 2s max) */
		500	/* StepSize = 0.5W */
	}
})

/* ========================================
 * PPCC (Power Performance Capability)
 * Définit les capacités exposées au kernel
 * ======================================== */
Name (PPCC, Package ()
{
	0x2,		/* Revision */
	
	/* PL1 Capabilities */
	Package () {
		0,	/* PowerLimitIndex = 0 */
		10000,	/* PowerLimitMinimum = 10W */
		60000,	/* PowerLimitMaximum = 60W */
		1000,	/* TimeWindowMinimum = 1s */
		32000,	/* TimeWindowMaximum = 32s */
		500	/* StepSize = 0.5W */
	},
	
	/* PL2 Capabilities */
	Package () {
		1,	/* PowerLimitIndex = 1 */
		15000,	/* PowerLimitMinimum = 15W */
		65000,	/* PowerLimitMaximum = 65W */
		0,	/* TimeWindowMinimum = 0 */
		2000,	/* TimeWindowMaximum = 2s */
		500	/* StepSize = 0.5W */
	}
})

/* ========================================
 * CLPO - SUPPRIMÉ
 * (peut forcer un bridage rigide)
 * ======================================== */

/* Include DPTF framework */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>