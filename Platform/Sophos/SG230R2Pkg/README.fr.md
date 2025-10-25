# Sophos SG230 Rev 2 - Firmware UEFI EDK2 Pur

Ceci est un firmware UEFI EDK2 pur pour le Sophos SG230 Rev 2, converti du port coreboot/edk2 original.

## Spécifications Matérielles

- **Chipset**: Intel H110
- **SuperIO**: Nuvoton NCT6779D
- **Intel ME**: Version Corporate avec HAP bit activé
- **Flash**: Flash SPI 16MB
- **Mémoire**: FSP 2.0 pour l'initialisation de la RAM
- **Support CPU**: CPUs Intel 6ème à 9ème génération (y compris Xeons)

## Organisation de la Flash

La flash de 16MB a l'organisation suivante:

```
Flash Region 0 (Flash Descriptor): 0x00000000 - 0x00000FFF (4KB)
Flash Region 1 (BIOS):             0x00A00000 - 0x00FFFFFF (~6MB)
Flash Region 2 (Intel ME):         0x00001000 - 0x009FFFFF (~10MB)
Flash Region 3 (GbE):              Non utilisée
Flash Region 4 (Platform Data):    Non utilisée
```

## Configuration de l'Environnement de Développement (Ubuntu/Debian)

### Prérequis

```bash
# Mettre à jour le système
sudo apt update && sudo apt upgrade -y

# Installer les paquets requis
sudo apt install -y build-essential uuid-dev iasl git nasm python3 python3-pip \
    python3-distutils gcc-multilib g++-multilib acpica-tools

# Optionnel: Installer VSCode
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/trusted.gpg.d/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
sudo apt update
sudo apt install -y code
```

### Obtenir EDK2

```bash
# Cloner le dépôt EDK2
cd ~
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init

# Cloner ce dépôt de plateforme
cd ..
git clone https://github.com/ulyssedu45/sg230.git
cd sg230

# Lier la plateforme à EDK2
ln -s $(pwd)/Platform/Sophos ~/edk2/Platform/
```

### Compilation du Firmware

```bash
# Configurer l'environnement EDK2
cd ~/edk2
source edksetup.sh

# Compiler BaseTools
make -C BaseTools

# Optionnel: Définir le nombre de jobs parallèles (ajuster selon vos cœurs CPU)
export MAKEFLAGS="-j$(nproc)"

# Compiler le firmware pour SG230 Rev 2
build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc

# Ou pour une compilation debug avec plus de détails:
build -a X64 -t GCC5 -b DEBUG -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc

# Ou utiliser le script de compilation fourni:
cd ~/edk2
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

Le firmware compilé sera situé à:
```
Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd
```

### Compilation avec les Binaires FSP

Vous devez obtenir les binaires Intel FSP 2.0 pour le chipset H110. Ils doivent être placés dans:

```bash
mkdir -p Platform/Sophos/SG230R2Pkg/FspBinPkg
# Copier les binaires FSP-M et FSP-S ici
```

Mettre à jour les chemins dans `SG230R2.dsc` si nécessaire:
```
gIntelFsp2WrapperTokenSpaceGuid.PcdFspmBaseAddress|0xFFF00000
gIntelFsp2WrapperTokenSpaceGuid.PcdFspsBaseAddress|0xFFE00000
```

## Configuration VSCode

Créer `.vscode/settings.json` dans votre répertoire EDK2:

```json
{
    "C_Cpp.default.includePath": [
        "${workspaceFolder}",
        "${workspaceFolder}/MdePkg/Include",
        "${workspaceFolder}/MdePkg/Include/X64",
        "${workspaceFolder}/MdeModulePkg/Include",
        "${workspaceFolder}/UefiCpuPkg/Include",
        "${workspaceFolder}/NetworkPkg/Include",
        "${workspaceFolder}/ShellPkg/Include",
        "${workspaceFolder}/Platform/Sophos/SG230R2Pkg/Include"
    ],
    "C_Cpp.default.defines": [
        "MDEPKG_NDEBUG",
        "DISABLE_NEW_DEPRECATED_INTERFACES"
    ],
    "C_Cpp.default.compilerPath": "/usr/bin/gcc",
    "C_Cpp.default.cStandard": "c11",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
    "files.associations": {
        "*.dsc": "ini",
        "*.fdf": "ini",
        "*.dec": "ini",
        "*.inf": "ini"
    }
}
```

Créer `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Compiler SG230R2 RELEASE",
            "type": "shell",
            "command": "./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        },
        {
            "label": "Compiler SG230R2 DEBUG",
            "type": "shell",
            "command": "./Platform/Sophos/SG230R2Pkg/build.sh DEBUG GCC5",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Nettoyer Build",
            "type": "shell",
            "command": "./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5 clean",
            "problemMatcher": []
        }
    ]
}
```

## Flasher le Firmware

### Prérequis
- Programmeur SPI (ex: CH341A, Raspberry Pi avec flashrom)
- Utilitaire flashrom
- Pince SOIC-8 ou adaptateur

### Sauvegarder le Firmware Original

```bash
# Toujours sauvegarder votre firmware original en premier!
sudo flashrom -p <programmeur> -r sg230r2_original.bin

# Vérifier la sauvegarde
sudo flashrom -p <programmeur> -v sg230r2_original.bin
```

### Flasher le Nouveau Firmware

**IMPORTANT**: Le firmware doit être combiné avec la région Intel ME et le descripteur de flash du firmware original.

```bash
# Extraire les régions du firmware original
dd if=sg230r2_original.bin of=descriptor.bin bs=1 count=4096 skip=0
dd if=sg230r2_original.bin of=me.bin bs=1 skip=4096 count=10481664

# Créer l'image finale (16MB)
dd if=/dev/zero of=sg230r2_uefi_final.bin bs=1M count=16

# Écrire le descripteur
dd if=descriptor.bin of=sg230r2_uefi_final.bin bs=1 seek=0 conv=notrunc

# Écrire la région ME
dd if=me.bin of=sg230r2_uefi_final.bin bs=1 seek=4096 conv=notrunc

# Écrire la région BIOS
dd if=Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd of=sg230r2_uefi_final.bin bs=1 seek=10485760 conv=notrunc

# Flasher l'image combinée
sudo flashrom -p <programmeur> -w sg230r2_uefi_final.bin
```

## Fonctionnalités

- Firmware UEFI pur (sans coreboot)
- Initialisation mémoire basée sur FSP 2.0
- Support de plusieurs générations de CPU (Skylake à Coffee Lake)
- Support ACPI complet
- Support USB 2.0/3.0
- Support SATA/AHCI
- Démarrage réseau (PXE, HTTP Boot, IPv4/IPv6)
- Compatible Secure Boot
- Shell UEFI inclus
- Support console série (COM1, 115200 baud)

## Dépannage

### Console Série

Connectez-vous à COM1 (115200 baud, 8N1) pour voir la sortie de débogage:

```bash
sudo screen /dev/ttyUSB0 115200
# ou
sudo minicom -D /dev/ttyUSB0 -b 115200
```

### Problèmes de Démarrage

1. Vérifiez la sortie série pour les erreurs
2. Vérifiez que les binaires FSP sont corrects pour votre CPU
3. Assurez-vous que la région ME est intacte et que le HAP bit est correctement configuré
4. Essayez de démarrer avec une configuration minimale (une seule barrette RAM, sans GPU discret)

### Problèmes de Compilation

```bash
# Tout nettoyer et recompiler
cd ~/edk2
source edksetup.sh
build clean
rm -rf Build/SG230R2
make -C BaseTools clean
make -C BaseTools
build -a X64 -t GCC5 -b RELEASE -p Platform/Sophos/SG230R2Pkg/SG230R2.dsc
```

## Contribuer

Les contributions sont les bienvenues! Veuillez soumettre des pull requests ou ouvrir des issues sur GitHub.

## Licence

Ce projet est sous licence BSD-2-Clause-Patent, suivant les conventions EDK2.

## Remerciements

- Port coreboot original par la communauté SG230
- Projet TianoCore EDK2
- Projet Intel FSP

## Avertissement

Ce firmware est fourni tel quel. Flasher un firmware peut bricker votre appareil. Conservez toujours une sauvegarde et procédez à vos risques et périls.

---

## Différences avec le Port Coreboot Original

Cette implémentation EDK2 pure diffère du port coreboot/edk2 de plusieurs manières:

1. **Architecture**: Utilise l'architecture native EDK2 au lieu de coreboot comme payload
2. **Initialisation**: FSP 2.0 gère l'initialisation matérielle directement
3. **ACPI**: Tables ACPI générées nativement par EDK2
4. **Boot**: Processus de démarrage UEFI standard sans étapes coreboot
5. **Configuration**: Fichiers DSC/FDF/DEC EDK2 au lieu de Kconfig

### Avantages

- Conformité UEFI native complète
- Meilleure intégration avec les systèmes d'exploitation modernes
- Support natif Secure Boot
- Plus facile à maintenir avec l'écosystème EDK2
- Meilleures performances de démarrage

### Migration

Si vous utilisez le port coreboot existant, vous devrez:

1. Sauvegarder votre configuration actuelle
2. Flasher le nouveau firmware UEFI
3. Reconfigurer les paramètres de démarrage dans l'UEFI Setup
4. Réinstaller le bootloader si nécessaire
