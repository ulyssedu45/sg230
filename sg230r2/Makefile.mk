# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += hwm_init.c
bootblock-y += ezio_lcd.c

romstage-y += ezio_lcd.c
romstage-y  += romstage.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += mainboard.c
ramstage-y += ramstage.c
ramstage-y += ezio_lcd.c
#ramstage-y += beep.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads