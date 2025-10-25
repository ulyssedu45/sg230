# Guide de Conversion Coreboot vers UEFI EDK2 - Sophos SG230 Rev 2

Ce document explique en détail la conversion effectuée du port coreboot/edk2 vers un firmware UEFI EDK2 pur.

## Vue d'Ensemble de la Conversion

### Architecture Avant (Coreboot)

```
┌─────────────────────────────────────────────────┐
│           Flash SPI 16MB                        │
├─────────────────────────────────────────────────┤
│ 0x000000 - 0x000FFF : Flash Descriptor          │
│ 0x001000 - 0x9FFFFF : Intel ME Region           │
│ 0xA00000 - 0xFFFFFF : BIOS Region (Coreboot)    │
│   ├─ Bootblock (Assembly init + SuperIO)       │
│   ├─ Romstage (RAM init via FSP-M)             │
│   ├─ Ramstage (Device init + FSP-S)            │
│   ├─ Payload (EDK2 DXE + BDS)                  │
│   └─ CBFS Metadata                              │
└─────────────────────────────────────────────────┘
```

### Architecture Après (EDK2 Pur)

```
┌─────────────────────────────────────────────────┐
│           Flash SPI 16MB                        │
├─────────────────────────────────────────────────┤
│ 0x000000 - 0x000FFF : Flash Descriptor          │
│ 0x001000 - 0x9FFFFF : Intel ME Region           │
│ 0xA00000 - 0xFFFFFF : BIOS Region (EDK2)        │
│   ├─ FV Recovery (SEC + PEI + FSP Wrapper)     │
│   ├─ FV Main (DXE Core + Drivers + BDS)        │
│   ├─ FV NvStorage (UEFI Variables)             │
│   └─ FV Microcode (CPU Microcode Updates)      │
└─────────────────────────────────────────────────┘
```

## Mapping des Composants

### 1. Initialisation Précoce

| Coreboot | EDK2 Pure | Fonction |
|----------|-----------|----------|
| bootblock.c | SecCore | Premier code exécuté |
| gpio.h | PlatformConfig.h | Configuration GPIO |
| SuperIO init (bootblock) | PEI Phase + SuperIO driver | Init SuperIO |

### 2. Initialisation RAM

| Coreboot | EDK2 Pure | Fonction |
|----------|-----------|----------|
| romstage.c | FspmWrapperPeim | Appel FSP-M |
| mainboard_memory_init_params() | UpdateFspmUpd() | Config mémoire |
| SPD reading | FSP Memory Init | Lecture SPD |

### 3. Initialisation Système

| Coreboot | EDK2 Pure | Fonction |
|----------|-----------|----------|
| ramstage.c | FspsWrapperPeim + DXE | Init silicon |
| mainboard_silicon_init_params() | UpdateFspsUpd() | Config FSP-S |
| devicetree.cb | Platform DSC/FDF | Config plateforme |

### 4. ACPI

| Coreboot | EDK2 Pure | Fonction |
|----------|-----------|----------|
| dsdt.asl | Dsdt.asl | Table DSDT |
| acpi/superio.asl | Intégré dans Dsdt.asl | SuperIO ACPI |
| acpi/mainboard.asl | Intégré dans Dsdt.asl | Board spécifique |

### 5. Payload/Boot

| Coreboot | EDK2 Pure | Fonction |
|----------|-----------|----------|
| Payload EDK2 | DXE Core natif | Core UEFI |
| Tianocore payload | BDS natif | Boot Device Selection |
| - | VariableRuntimeDxe | Variables UEFI |

## Fichiers de Configuration Convertis

### devicetree.cb → SG230R2.dsc

**Avant (Coreboot devicetree.cb)**:
```c
chip soc/intel/skylake
    register "eist_enable" = "true"
    register "PrimaryDisplay" = "Display_PEG"
    register "SendVrMbxCmd" = "2"
    
    device domain 0 on
        device ref system_agent on
            subsystemid 0x1849 0x191f
        end
        device ref peg0 on
            subsystemid 0x1849 0x1901
            register "Peg0MaxLinkWidth" = "Peg0_x16"
        end
        # ...
    end
end
```

**Après (EDK2 SG230R2.dsc)**:
```ini
[PcdsFixedAtBuild]
  # Configuration équivalente via PCDs et FSP UPD
  gIntelFsp2WrapperTokenSpaceGuid.PcdFspmBaseAddress|0xFFF00000
  gIntelFsp2WrapperTokenSpaceGuid.PcdFspsBaseAddress|0xFFE00000
  
[Components.X64]
  IntelFsp2WrapperPkg/FspmWrapperPeim/FspmWrapperPeim.inf
  IntelFsp2WrapperPkg/FspsWrapperPeim/FspsWrapperPeim.inf
```

### Kconfig → DEC/DSC

**Avant (Kconfig)**:
```kconfig
config BOARD_SPECIFIC_OPTIONS
    def_bool y
    select BOARD_ROMSIZE_KB_16384
    select HAVE_ACPI_TABLES
    select INTEL_GMA_HAVE_VBT
    select SKYLAKE_SOC_PCH_H
    select SOC_INTEL_KABYLAKE
```

**Après (SG230R2Pkg.dec + SG230R2.dsc)**:
```ini
# SG230R2Pkg.dec
[Defines]
  PACKAGE_NAME = SG230R2Pkg
  PACKAGE_VERSION = 0.1

# SG230R2.dsc
[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  # Équivalent des sélections Kconfig
```

## Changements de Code Majeurs

### 1. Initialisation SuperIO

**Avant (bootblock.c)**:
```c
static void early_config_superio(void)
{
    nuvoton_pnp_enter_conf_state(GLOBAL_DEV);
    pnp_write_config(GLOBAL_DEV, 0x10, 0xFF);
    // ... configuration manuelle
    nuvoton_pnp_exit_conf_state(GLOBAL_DEV);
    nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
```

**Après (PEI Phase ou DXE Driver)**:
```c
// Approche EDK2: Driver SuperIO ou PlatformPei
EFI_STATUS
EFIAPI
PlatformPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  // Init SuperIO via I/O
  IoWrite8(NCT6779D_INDEX_PORT, 0x87);
  IoWrite8(NCT6779D_INDEX_PORT, 0x87);
  // Configuration...
  IoWrite8(NCT6779D_INDEX_PORT, 0xAA);
}
```

### 2. Configuration Mémoire

**Avant (romstage.c)**:
```c
void mainboard_memory_init_params(FSPM_UPD *mupd)
{
    const uint16_t rcomp_resistors[3] = { 121, 75, 100 };
    FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
    
    mem_cfg->DqPinsInterleaved = 1;
    mem_cfg->CaVrefConfig = 2;
    get_spd_smbus(&blk);
    mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
}
```

**Après (FSP Wrapper Library)**:
```c
EFI_STATUS
EFIAPI
UpdateFspmUpd (
  IN OUT FSPM_UPD  *FspmUpd
  )
{
  FSPM_CONFIG  *FspmConfig;
  
  FspmConfig = &FspmUpd->FspmConfig;
  FspmConfig->DqPinsInterleaved = 1;
  FspmConfig->CaVrefConfig = 2;
  
  // SPD addresses
  FspmConfig->MemorySpdPtr00 = 0;  // FSP reads via I2C
  FspmConfig->SpdAddressTable[0] = 0x50;
  FspmConfig->SpdAddressTable[1] = 0x51;
  
  return EFI_SUCCESS;
}
```

### 3. ACPI Tables

**Avant (dsdt.asl avec includes coreboot)**:
```asl
#include <acpi/acpi.h>
DefinitionBlock(...)
{
    #include <acpi/dsdt_top.asl>
    #include <soc/intel/common/block/acpi/acpi/platform.asl>
    #include <cpu/intel/common/acpi/cpu.asl>
    // Includes coreboot-spécifiques
}
```

**Après (Dsdt.asl EDK2 standalone)**:
```asl
DefinitionBlock (
  "Dsdt.aml",
  "DSDT",
  2,
  "SOPHOS",
  "SG230R2",
  0x20240101
)
{
  Scope(\_SB) {
    Device(PCI0) {
      Name(_HID, EISAID("PNP0A08"))
      // Définitions complètes sans dépendances externes
    }
  }
}
```

## Build System

### Avant (Coreboot)

```bash
# Configuration
make menuconfig

# Compilation
make crossgcc-i386 CPUS=$(nproc)
make

# Résultat
build/coreboot.rom
```

### Après (EDK2)

```bash
# Setup
source edksetup.sh
make -C BaseTools

# Compilation
build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc

# Résultat
Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd
```

## Avantages de la Conversion

### 1. Conformité UEFI Native
- ✅ Pas de couche d'émulation
- ✅ Support complet UEFI 2.x
- ✅ Meilleure compatibilité OS

### 2. Maintenance
- ✅ Suit l'écosystème EDK2 mainstream
- ✅ Mises à jour de sécurité plus faciles
- ✅ Meilleure documentation

### 3. Fonctionnalités
- ✅ Secure Boot natif
- ✅ Variables UEFI persistantes
- ✅ Network boot moderne (HTTP Boot)
- ✅ GOP (Graphics Output Protocol)

### 4. Performance
- ✅ Démarrage plus rapide (moins de couches)
- ✅ Moins de code à exécuter
- ✅ Meilleure gestion mémoire

## Défis et Solutions

### Défi 1: SuperIO NCT6779D
**Problème**: Pas de driver EDK2 existant
**Solution**: Configuration directe via I/O en PEI phase, déclaration ACPI pour l'OS

### Défi 2: FSP Integration
**Problème**: Besoin de FSP 2.0 binaires
**Solution**: Utilisation de FSP Wrapper de Intel, configuration via UPD structures

### Défi 3: GPIO Configuration
**Problème**: Conversion du format coreboot
**Solution**: Extraction et conversion en structures EDK2 standard

### Défi 4: Flash Layout
**Problème**: Organisation différente
**Solution**: FDF file avec régions définies, compatible avec layout original

## Tests et Validation

### Checklist de Test

- [ ] POST complet (Power-On Self Test)
- [ ] Détection RAM correcte
- [ ] Énumération USB
- [ ] Détection SATA
- [ ] Boot depuis USB
- [ ] Boot depuis SATA
- [ ] Network boot (PXE)
- [ ] Console série fonctionnelle
- [ ] UEFI Shell accessible
- [ ] Variables UEFI persistantes
- [ ] Boot Windows 10/11
- [ ] Boot Linux (Ubuntu/Debian)
- [ ] Sortie vidéo GOP

### Outils de Debug

1. **Console Série**: Output debug via COM1
2. **UEFI Shell**: Commands pour inspection
3. **Debug Builds**: Build avec symboles de debug
4. **FSP Debug**: Activer FSP debug output

## Migration des Utilisateurs

### Étape 1: Préparation
```bash
# Sauvegarder firmware actuel
flashrom -p <programmer> -r backup.bin

# Extraire ME et descriptor
dd if=backup.bin of=me.bin bs=1 skip=4096 count=10481664
dd if=backup.bin of=descriptor.bin bs=1 count=4096
```

### Étape 2: Compilation EDK2
```bash
cd ~/edk2
source edksetup.sh
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

### Étape 3: Assemblage
```bash
# Créer image finale
dd if=/dev/zero of=final.bin bs=1M count=16
dd if=descriptor.bin of=final.bin bs=1 seek=0 conv=notrunc
dd if=me.bin of=final.bin bs=1 seek=4096 conv=notrunc
dd if=Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd of=final.bin bs=1 seek=10485760 conv=notrunc
```

### Étape 4: Flash
```bash
flashrom -p <programmer> -w final.bin
```

## Ressources

### Documentation
- [EDK2 Documentation](https://github.com/tianocore/tianocore.github.io/wiki)
- [UEFI Specification](https://uefi.org/specifications)
- [Intel FSP](https://github.com/IntelFsp/FSP)

### Outils
- [flashrom](https://www.flashrom.org/)
- [UEFITool](https://github.com/LongSoft/UEFITool)
- [me_cleaner](https://github.com/corna/me_cleaner)

## Conclusion

Cette conversion transforme le Sophos SG230 Rev 2 d'une plateforme coreboot+payload en une véritable plateforme UEFI native, offrant une meilleure compatibilité, performance et maintenabilité.

Le processus est complexe mais bien documenté, et le résultat est un firmware moderne qui suit les standards de l'industrie.
