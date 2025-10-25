# Sophos SG230 Rev 2 - Conversion Complete du Port Coreboot vers UEFI EDK2 Pur

Ce dépôt contient maintenant:
1. Le port coreboot/edk2 original dans `sg230r2/`
2. **NOUVEAU**: Le port UEFI EDK2 pur dans `Platform/Sophos/SG230R2Pkg/`

## Qu'est-ce qui a changé?

Le firmware d'origine était basé sur **coreboot** avec un payload EDK2. Cette nouvelle version est un **firmware UEFI EDK2 pur** qui n'utilise plus coreboot.

### Port Coreboot Original

```
[Flash] → [Coreboot] → [FSP] → [EDK2 Payload] → [OS]
```

### Nouveau Port UEFI EDK2 Pur

```
[Flash] → [EDK2 SEC/PEI] → [FSP 2.0] → [EDK2 DXE/BDS] → [OS]
```

## Démarrage Rapide

### Pour utiliser le nouveau firmware UEFI EDK2:

```bash
# Installation de l'environnement
cd ~
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init
source edksetup.sh
make -C BaseTools

# Cloner et lier la plateforme
cd ~
git clone https://github.com/ulyssedu45/sg230.git
ln -s ~/sg230/Platform/Sophos ~/edk2/Platform/

# Compiler
cd ~/edk2
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

### Documentation complète:
- **Français**: [Platform/Sophos/SG230R2Pkg/README.fr.md](Platform/Sophos/SG230R2Pkg/README.fr.md)
- **English**: [Platform/Sophos/SG230R2Pkg/README.md](Platform/Sophos/SG230R2Pkg/README.md)

## Architecture du Nouveau Port

### Structure des Fichiers

```
Platform/Sophos/SG230R2Pkg/
├── SG230R2.dsc           # Description de la plateforme (comme Kconfig)
├── SG230R2.fdf           # Organisation de la flash
├── SG230R2Pkg.dec        # Déclarations du package
├── Include/              # Headers de la plateforme
│   └── PlatformConfig.h
├── AcpiTables/           # Tables ACPI
│   ├── Dsdt.asl
│   └── AcpiTables.inf
├── build.sh              # Script de compilation
├── README.md             # Documentation (EN)
└── README.fr.md          # Documentation (FR)
```

### Organisation de la Flash (16MB)

```
0x00000000 - 0x00000FFF : Flash Descriptor (4KB)
0x00001000 - 0x009FFFFF : Intel ME (~10MB)
0x00A00000 - 0x00FFFFFF : BIOS UEFI (~6MB)
  ├── 0x00A00000 - 0x00AFFFFF : Recovery FV (SEC+PEI)
  ├── 0x00B00000 - 0x00EFFFFF : Main FV (DXE)
  ├── 0x00F00000 - 0x00F3FFFF : NV Storage (Variables)
  └── 0x00F40000 - 0x00FFFFFF : Microcode
```

## Fonctionnalités

### Hardware Support
- ✅ Intel H110 Chipset (Skylake/Kaby Lake/Coffee Lake)
- ✅ Nuvoton NCT6779D SuperIO
- ✅ Intel ME avec HAP bit
- ✅ FSP 2.0 pour initialisation RAM
- ✅ Support 6ème à 9ème gen CPUs (y compris Xeons)

### UEFI Features
- ✅ Boot UEFI natif
- ✅ Secure Boot capable
- ✅ GOP (Graphics Output Protocol)
- ✅ ACPI complet
- ✅ SMBIOS
- ✅ Variables UEFI persistantes
- ✅ Shell UEFI

### Périphériques
- ✅ USB 2.0/3.0 (XHCI/EHCI)
- ✅ SATA/AHCI
- ✅ PCIe
- ✅ GbE Network
- ✅ Console série (COM1/COM2)
- ✅ PS/2 Clavier/Souris

### Boot Options
- ✅ PXE Boot (IPv4/IPv6)
- ✅ HTTP Boot
- ✅ Boot depuis USB/SATA/NVMe
- ✅ UEFI Shell

## Prérequis pour la Compilation

### Ubuntu/Debian
```bash
sudo apt install -y build-essential uuid-dev iasl git nasm \
    python3 python3-pip gcc-multilib g++-multilib acpica-tools
```

## Avantages du Port UEFI Pur

1. **Conformité UEFI Native**: Pas de couche d'émulation coreboot
2. **Meilleure Compatibilité OS**: Windows, Linux, BSD supportent nativement UEFI
3. **Secure Boot**: Support natif pour Secure Boot
4. **Performance**: Démarrage plus rapide sans couches intermédiaires
5. **Maintenance**: Suit l'écosystème EDK2 mainstream
6. **Extensibilité**: Plus facile d'ajouter de nouveaux drivers UEFI

## Migration depuis Coreboot

Si vous utilisez actuellement le port coreboot:

1. **Sauvegardez** votre firmware actuel avec flashrom
2. **Extrayez** les régions ME et descriptor
3. **Compilez** le nouveau firmware UEFI
4. **Combinez** les régions (voir documentation)
5. **Flashez** avec précaution

⚠️ **IMPORTANT**: Gardez toujours une sauvegarde et un moyen de reflasher!

## Configuration Intel ME

Le firmware nécessite Intel ME avec HAP (High Assurance Platform) bit activé. Vous pouvez utiliser `me_cleaner` ou `intelmetool` pour configurer cela.

```bash
# Exemple avec me_cleaner
python me_cleaner.py -S -O sg230r2_me_cleaned.bin sg230r2_original.bin
```

## Support et Développement

### VSCode Configuration

Le dépôt inclut une configuration VSCode complète:
- IntelliSense pour les headers EDK2
- Tâches de compilation intégrées
- Coloration syntaxique pour DSC/FDF/INF

### Compiler dans VSCode

1. Ouvrir le dossier `~/edk2` dans VSCode
2. Appuyer sur `Ctrl+Shift+B`
3. Sélectionner "Compiler SG230R2 RELEASE"

## Roadmap

- [x] Structure de base EDK2
- [x] Configuration FSP 2.0
- [x] Tables ACPI
- [x] Support GPIO
- [x] Support SuperIO NCT6779D
- [x] Documentation complète
- [ ] Platform Boot Manager personnalisé
- [ ] Logo de démarrage
- [ ] Drivers SuperIO avancés (HWM, Fan Control)
- [ ] Tests de compatibilité CPU étendus
- [ ] Support DPTF (Dynamic Platform Thermal Framework)

## Dépannage

### La compilation échoue
```bash
cd ~/edk2
source edksetup.sh
make -C BaseTools clean
make -C BaseTools
build clean
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

### Le système ne démarre pas
1. Vérifiez la console série (COM1, 115200 8N1)
2. Vérifiez que le ME est intact
3. Testez avec une configuration minimale (1 RAM, pas de GPU)
4. Vérifiez que les binaires FSP sont corrects

### Pas de sortie vidéo
1. Le GOP devrait fonctionner avec IGPU Intel
2. Pour GPU discrets, attendez que l'OS charge les drivers
3. Utilisez la console série pour le débogage initial

## Contribuer

Les contributions sont bienvenues!

1. Fork le dépôt
2. Créez une branche feature (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une Pull Request

## Licence

- **EDK2 Platform**: BSD-2-Clause-Patent
- **Port Coreboot Original**: GPL-2.0-only

## Remerciements

- Communauté Sophos SG230
- Projet TianoCore EDK2
- Projet Coreboot
- Intel FSP
- Nuvoton pour la documentation NCT6779D

## Liens Utiles

- [EDK2 Documentation](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)
- [Intel FSP](https://github.com/IntelFsp/FSP)
- [UEFI Specification](https://uefi.org/specifications)
- [Flashrom](https://www.flashrom.org/)

---

**Note**: Ce port est un projet communautaire non affilié à Sophos Ltd.
