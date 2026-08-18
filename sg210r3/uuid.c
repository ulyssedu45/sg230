/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/resource.h>
#include <smbios.h>
#include <stdio.h>
#include <string.h>
#include <uuid.h>

/*
 * Intel I211 Receive Address Register 0
 *
 * RAL0 = MAC bytes 0..3
 * RAH0 = MAC bytes 4..5
 */
#define E1000_RAL0		0x05400
#define E1000_RAH0		0x05404
#define E1000_RAH_AV		0x80000000U

static u8 cached_mac[6];
static int cached_mac_state; /* 0 = inconnu, 1 = OK, -1 = erreur */

static char system_serial[32];

static int mac_valid(const u8 mac[6])
{
	int all_zero = 1;
	int all_ff = 1;
	int i;

	for (i = 0; i < 6; i++) {
		if (mac[i] != 0x00)
			all_zero = 0;

		if (mac[i] != 0xff)
			all_ff = 0;
	}

	/* Adresse multicast => invalide pour notre usage. */
	if (mac[0] & 0x01)
		return 0;

	if (all_zero || all_ff)
		return 0;

	return 1;
}

static const struct device *get_pci_root_device(const struct device *dev)
{
	const struct device *cur = dev;

	/*
	 * Remonte les bridges PCI jusqu'au premier périphérique PCI
	 * directement sous le domaine/root bus.
	 */
	while (cur->upstream &&
	       cur->upstream->dev &&
	       cur->upstream->dev->path.type == DEVICE_PATH_PCI) {
		cur = cur->upstream->dev;
	}

	return cur;
}

static int get_uuid_mac(u8 mac[6])
{
	struct device *dev;
	const struct device *best_root = NULL;
	struct device *best_dev = NULL;

	u8 best_mac[6];

	if (cached_mac_state == 1) {
		memcpy(mac, cached_mac, 6);
		return 0;
	}

	if (cached_mac_state == -1)
		return -1;

	for (dev = all_devices; dev; dev = dev->next) {
		const struct device *root;
		struct resource *bar0;
		u8 *mmio;
		u32 ral;
		u32 rah;
		u8 candidate_mac[6];

		/* PCI uniquement */
		if (dev->path.type != DEVICE_PATH_PCI)
			continue;

		/* Ethernet uniquement */
		if ((dev->class >> 8) != PCI_CLASS_NETWORK_ETHERNET)
			continue;

		/* Intel uniquement */
		if (dev->vendor != 0x8086)
			continue;

		root = get_pci_root_device(dev);

		if (!root ||
		    root->path.type != DEVICE_PATH_PCI)
			continue;

		printk(BIOS_DEBUG,
		       "SMBIOS: candidate %s [8086:%04x], root=%s\n",
		       dev_path(dev),
		       dev->device,
		       dev_path(root));

		/*
		 * Si on a déjà trouvé une carte derrière un Root Port
		 * plus petit, inutile d'essayer celle-ci.
		 */
		if (best_root &&
		    root->path.pci.devfn > best_root->path.pci.devfn)
			continue;

		bar0 = probe_resource(dev, PCI_BASE_ADDRESS_0);

		if (!bar0)
			continue;

		if (!(bar0->flags & IORESOURCE_MEM))
			continue;

		if (!(bar0->flags & IORESOURCE_ASSIGNED))
			continue;

		mmio = res2mmio(bar0, 0, 0);

		ral = read32(mmio + E1000_RAL0);
		rah = read32(mmio + E1000_RAH0);

		if (!(rah & E1000_RAH_AV))
			continue;

		candidate_mac[0] = (ral >> 0)  & 0xff;
		candidate_mac[1] = (ral >> 8)  & 0xff;
		candidate_mac[2] = (ral >> 16) & 0xff;
		candidate_mac[3] = (ral >> 24) & 0xff;
		candidate_mac[4] = (rah >> 0)  & 0xff;
		candidate_mac[5] = (rah >> 8)  & 0xff;

		if (!mac_valid(candidate_mac))
			continue;

		/*
		 * Nouveau meilleur candidat si son Root Port est plus petit.
		 */
		if (!best_root ||
		    root->path.pci.devfn < best_root->path.pci.devfn) {

			best_root = root;
			best_dev = dev;

			memcpy(best_mac, candidate_mac, 6);
		}
	}

	if (!best_dev) {
		printk(BIOS_ERR,
		       "SMBIOS: no readable Intel Ethernet controller found\n");

		cached_mac_state = -1;
		return -1;
	}

	memcpy(cached_mac, best_mac, 6);
	memcpy(mac, best_mac, 6);

	cached_mac_state = 1;

	printk(BIOS_INFO,
	       "SMBIOS: selected identity NIC %s via root %s\n",
	       dev_path(best_dev),
	       dev_path(best_root));

	printk(BIOS_INFO,
	       "SMBIOS: identity MAC "
	       "%02x:%02x:%02x:%02x:%02x:%02x\n",
	       mac[0], mac[1], mac[2],
	       mac[3], mac[4], mac[5]);

	return 0;
}

/*
 * Génère un UUIDv8 déterministe :
 *
 * 534f5048-4f53-8001-8001-xxxxxxxxxxxx
 * |----------|             |
 *   "SOPHOS"               +-- MAC
 *
 * Exemple :
 *
 * MAC:
 * 7c:5a:1c:4d:68:a0
 *
 * UUID:
 * 534f5048-4f53-8001-8001-7c5a1c4d68a0
 */
void smbios_system_set_uuid(u8 *uuid)
{
	u8 mac[6];
	char uuid_str[UUID_STRLEN + 1];

	/*
	 * En cas d'erreur, garder le comportement coreboot par défaut :
	 * UUID tout à zéro.
	 */
	memset(uuid, 0, UUID_LEN);

	if (get_uuid_mac(mac))
		return;

	snprintf(uuid_str, sizeof(uuid_str),
		 "534f5048-4f53-8001-8001-"
		 "%02x%02x%02x%02x%02x%02x",
		 mac[0], mac[1], mac[2],
		 mac[3], mac[4], mac[5]);

	printk(BIOS_INFO,
	       "SMBIOS UUID: generated %s\n",
	       uuid_str);

	/*
	 * Important :
	 *
	 * utiliser parse_uuid() permet à coreboot de faire lui-même
	 * la conversion vers l'ordre d'octets SMBIOS correct.
	 */
	if (parse_uuid(uuid, uuid_str)) {
		printk(BIOS_ERR,
		       "SMBIOS UUID: parse_uuid() failed\n");

		memset(uuid, 0, UUID_LEN);
	}
}

const char *smbios_system_serial_number(void)
{
	u8 mac[6];

	if (get_uuid_mac(mac))
		return CONFIG_MAINBOARD_SERIAL_NUMBER;

	snprintf(system_serial, sizeof(system_serial),
		 "SG230-%02X%02X%02X%02X%02X%02X",
		 mac[0], mac[1], mac[2],
		 mac[3], mac[4], mac[5]);

	printk(BIOS_INFO,
	       "SMBIOS: system serial number %s\n",
	       system_serial);

	return system_serial;
}

const char *smbios_mainboard_serial_number(void)
{
	return smbios_system_serial_number();
}
