# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += hwm_init.c

romstage-y  += romstage.c

ramstage-y += mainboard.c
ramstage-y += ramstage.c
ramstage-y += beep.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads