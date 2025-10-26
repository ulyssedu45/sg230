/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/romstage.h>
#include <soc/pm.h>
#include <stdint.h>
#include <string.h>
#include <spd_bin.h>
#include <arch/io.h>
#include <console/console.h>
#include <fsp/api.h>
#include <delay.h>
#include <device/pnp_ops.h>
#include <option.h>
void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	
	printk(BIOS_INFO, "SG230R2: Vérification de la RTC...\n");
	if(rtc_failure()){
		printk(BIOS_ERR, "rtc_failure!\n");
		set_uint_option("igd_enabled", !CONFIG(SOC_INTEL_DISABLE_IGD));

		printk(BIOS_EMERG, "Exécution d'un cold reset...\n");
    
		/* Méthode 1: Reset via le port 0xCF9 (PCH reset control) */
		outb(0x0E, 0xCF9);  /* Full reset + CPU reset */
		
		/* Si ça ne marche pas, essayer le reset classique */
		outb(0x06, 0xCF9);
		
		/* Attendre indéfiniment (le système devrait redémarrer) */
		for (;;) {
			asm volatile ("hlt");
		}
	}
	

	const uint16_t rcomp_resistors[3] = { 121, 75, 100 };
	const uint16_t rcomp_targets[5] = { 50, 26, 20, 20, 26 };

	FSP_M_CONFIG *const mem_cfg = &mupd->FspmConfig;

	struct spd_block blk = {
		.addr_map = { 0x50, 0x51 },
	};

	printk(BIOS_INFO, "SG230R2: Configuration mémoire...\n");

	assert(sizeof(mem_cfg->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(mem_cfg->RcompTarget)   == sizeof(rcomp_targets));

	mem_cfg->DqPinsInterleaved = 1;
	mem_cfg->CaVrefConfig = 2;

	get_spd_smbus(&blk);
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (uintptr_t)blk.spd_array[1];
	dump_spd_info(&blk);

	memcpy(mem_cfg->RcompResistor, rcomp_resistors, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget,   rcomp_targets,   sizeof(mem_cfg->RcompTarget));

	/* Use virtual channel 1 for DMI interface */
	mupd->FspmTestConfig.DmiVc1 = 1;
}
