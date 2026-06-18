# Coreboot pour Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2 / Coreboot for Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2

[🇫🇷 Version Française](#version-française) | [🇬🇧 English Version](#english-version)

---

## Version Française

Port coreboot pour le Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2, un appareil de sécurité réseau basé sur Intel Skylake.

## ⚠️ Avertissements Importants

### Sauvegarde du BIOS Original

**IMPORTANT** : Avant de flasher coreboot, vous **devez** faire une sauvegarde complète du BIOS d'origine. Cette sauvegarde est essentielle pour restaurer le système en cas de problème.

> **Note** : La lecture du BIOS d'origine peut être effectuée directement depuis le système d'exploitation avec flashrom via le programmateur interne (`-p internal`). Il est fortement recommandé de réaliser **plusieurs lectures** et de comparer leurs empreintes pour s'assurer de l'intégrité de la sauvegarde :
>
> ```bash
> ./flashrom -p internal -r xg230_bios_1.bin
> ./flashrom -p internal -r xg230_bios_2.bin
> sha256 xg230_bios_1.bin xg230_bios_2.bin
> ```

### Programmateur Externe Requis

La première installation de coreboot nécessite **obligatoirement** un programmateur externe (par exemple CH341A, Raspberry Pi, Bus Pirate, etc.) car la puce BIOS n'est pas socketed.

Bien que flashrom soit capable de **lire** le BIOS d'origine via le programmateur interne, il **n'est pas en mesure de lever le verrouillage (lock-down)** mis en place par le BIOS OEM AMI: la configuration SPI est verrouée, le BIOS Interface Lock-Down est activé et le bit BIOS_CNTL Write Enable ne peut pas être positionné au runtime. Pour cette raison, **le programmateur interne ne peut pas écrire sur la puce BIOS d'origine**, ce qui rend l'utilisation d'un programmateur externe indispensable pour la première installation.

Une fois coreboot installé, les mises à jour futures peuvent être effectuées via flashrom depuis le système d'exploitation.

> [!NOTE]
> Sur certains modèles ou certaines révisions, j'ai constaté que le flash pouvait être effectué directement avec `flashrom` via le programmateur interne.
> Cela dépend de la configuration exacte du firmware OEM et ne doit donc pas être considéré comme garanti.

## 📋 Spécifications

- **Modèle** : Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2
- **Année de sortie** : 2017
- **Catégorie** : Desktop
- **SOC** : Intel Skylake (Skylake PCH-H)
- **Puce ROM** : SOIC-8
- **Protocole ROM** : SPI
- **Taille ROM** : 16 MB (16384 KB)
- **Superio** : Nuvoton NCT6779D
- **DIMM Max** : 2

## 🔧 Fonctionnalités

- ✅ Support ACPI complet avec reprise depuis veille
- ✅ Initialisation graphique avec libgfxinit
- ✅ VBT Intel GMA inclus
- ✅ Support USB ACPI
- ✅ Timer legacy 8254
- ✅ Support CFR (Configuration File Region) pour EDK2 avec SMMSTORE
- ✅ Hardware monitoring (NCT6779D)

## 🛠️ Construction

### Configuration et Compilation

Pour configurer et compiler coreboot pour le SG210/XG210 Rev 3 & SG230/XG230 Rev 2, veuillez suivre les instructions officielles de compilation disponibles sur le site de coreboot :

👉 **[Guide officiel de compilation coreboot](https://doc.coreboot.org/tutorial/part1.html)**

**Étapes spécifiques pour le SG210/XG210 Rev 3 & SG230/XG230 Rev 2 :**

1. Cloner le dépôt coreboot et préparer l'environnement (voir guide officiel)

2. Copier les fichiers du mainboard dans l'arborescence coreboot :

```bash
cp -r chemin/vers/sg230r2 src/mainboard/sophos/
```

3. Lors de la configuration (`make menuconfig`), sélectionner :
   - **Mainboard vendor** : `Sophos`
   - **Mainboard model** : `SG210/XG210 Rev 3 & SG230/XG230 Rev 2`
   - Configurer le payload selon vos besoins

4. Compiler selon les instructions du guide officiel

## 📦 Installation

### Première Installation (Programmateur Externe Requis)

1. **Sauvegarder le BIOS original** :

```bash
flashrom -p <votre_programmateur> -r backup_original.bin
flashrom -p <votre_programmateur> -r backup_original2.bin
# Vérifier que les deux sauvegardes sont identiques
diff backup_original.bin backup_original2.bin
```

2. **Conserver ces sauvegardes en lieu sûr** (cloud, disque externe, etc.)

3. Flasher coreboot :

```bash
flashrom -p <votre_programmateur> -w coreboot.rom
```

### Mises à Jour Ultérieures

Une fois coreboot installé, vous pouvez utiliser flashrom en interne :

```bash
flashrom -p internal -w coreboot.rom
```

## 📁 Structure du Projet

```
sg230r2/
├── bootblock.c         # Code du bootblock
├── ramstage.c          # Code du ramstage
├── romstage.c          # Code du romstage
├── mainboard.c         # Configuration principale du mainboard
├── devicetree.cb       # Arbre des périphériques
├── gpio.h              # Configuration GPIO
├── hwm_init.c/h        # Initialisation hardware monitoring
├── gma-mainboard.ads   # Configuration graphique
├── data.vbt            # Video BIOS Table
├── dsdt.asl            # Tables ACPI
├── cfr.c               # Configuration File Region
├── die.c               # Gestion des erreurs
├── Kconfig             # Configuration Kconfig
└── acpi/               # Fichiers ACPI supplémentaires
    ├── ec.asl
    ├── mainboard.asl
    └── superio.asl
```

## 📝 Licence

Ce projet est **totalement libre de droit** et peut être utilisé, modifié et distribué librement sans aucune restriction.

Le code coreboot original est sous licence GPL-2.0.

## 🤝 Contribution

Les contributions sont les bienvenues ! N'hésitez pas à ouvrir des issues ou des pull requests.

## 📚 Ressources

- [Documentation Coreboot](https://doc.coreboot.org/)
- [Guide des programmateurs externes](https://www.flashrom.org/)

## ⚠️ Disclaimer

Ce firmware est fourni "tel quel" sans garantie d'aucune sorte. L'utilisation de ce firmware est à vos propres risques. Assurez-vous toujours d'avoir une sauvegarde de votre BIOS original avant de procéder.

---

## English Version

Coreboot port for the Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2, a network security appliance based on Intel Skylake.

## ⚠️ Important Warnings

### Original BIOS Backup

**IMPORTANT**: Before flashing coreboot, you **must** make a complete backup of the original BIOS. This backup is essential to restore the system in case of problems.

> **Note**: The original BIOS can be read directly from the operating system using flashrom via the internal programmer (`-p internal`). It is strongly recommended to take **multiple reads** and compare their hashes to ensure the backup is consistent:
>
> ```bash
> ./flashrom -p internal -r xg230_bios_1.bin
> ./flashrom -p internal -r xg230_bios_2.bin
> sha256 xg230_bios_1.bin xg230_bios_2.bin
> ```

### External Programmer Required

The first coreboot installation **requires** an external programmer (e.g., CH341A, Raspberry Pi, Bus Pirate, etc.) because the BIOS chip is not socketed.

Although flashrom is able to **read** the original BIOS through the internal programmer, it **cannot undo the lock-down** enforced by the OEM AMI BIOS: the SPI configuration is locked, BIOS Interface Lock-Down is set, and the BIOS_CNTL Write Enable bit cannot be set at runtime. As a result, **the internal programmer cannot write to the original BIOS chip**, which is precisely why an external programmer is required for the first installation.

Once coreboot is installed, future updates can be performed via flashrom from the operating system.

> [!NOTE]
> On some models or board revisions, I have observed that flashing directly with `flashrom` through the internal programmer may be possible.
> This depends on the exact OEM firmware configuration and should therefore not be considered guaranteed.

## 📋 Specifications

- **Model**: Sophos SG210/XG210 Rev 3 & SG230/XG230 Rev 2
- **Release Year**: 2017
- **Category**: Desktop
- **SOC**: Intel Skylake (Skylake PCH-H)
- **ROM Chip**: SOIC-8
- **ROM Protocol**: SPI
- **ROM Size**: 16 MB (16384 KB)
- **Superio**: Nuvoton NCT6779D
- **DIMM Max**: 2

## 🔧 Features

- ✅ Full ACPI support with resume from sleep
- ✅ Graphics initialization with libgfxinit
- ✅ Intel GMA VBT included
- ✅ USB ACPI support
- ✅ Legacy 8254 timer
- ✅ CFR (Configuration File Region) support for EDK2 with SMMSTORE
- ✅ Hardware monitoring (NCT6779D)

## 🛠️ Building

### Configuration and Compilation

To configure and compile coreboot for the SG210/XG210 Rev 3 & SG230/XG230 Rev 2, please follow the official compilation instructions available on the coreboot website:

👉 **[Official coreboot build guide](https://doc.coreboot.org/tutorial/part1.html)**

**Specific steps for the SG210/XG210 Rev 3 & SG230/XG230 Rev 2:**

1. Clone the coreboot repository and prepare the environment (see official guide)

2. Copy the mainboard files into the coreboot tree:

```bash
cp -r path/to/sg230r2 src/mainboard/sophos/
```

3. During configuration (`make menuconfig`), select:
   - **Mainboard vendor**: `Sophos`
   - **Mainboard model**: `SG210/XG210 Rev 3 & SG230/XG230 Rev 2`
   - Configure the payload according to your needs

4. Compile according to the official guide instructions

## 📦 Installation

### First Installation (External Programmer Required)

1. **Backup the original BIOS**:

```bash
flashrom -p <your_programmer> -r backup_original.bin
flashrom -p <your_programmer> -r backup_original2.bin
# Verify that both backups are identical
diff backup_original.bin backup_original2.bin
```

2. **Keep these backups in a safe place** (cloud, external drive, etc.)

3. Flash coreboot:

```bash
flashrom -p <your_programmer> -w coreboot.rom
```

### Subsequent Updates

Once coreboot is installed, you can use internal flashrom:

```bash
flashrom -p internal -w coreboot.rom
```

## 📁 Project Structure

```
sg230r2/
├── bootblock.c         # Bootblock code
├── ramstage.c          # Ramstage code
├── romstage.c          # Romstage code
├── mainboard.c         # Main mainboard configuration
├── devicetree.cb       # Device tree
├── gpio.h              # GPIO configuration
├── hwm_init.c/h        # Hardware monitoring initialization
├── gma-mainboard.ads   # Graphics configuration
├── data.vbt            # Video BIOS Table
├── dsdt.asl            # ACPI tables
├── cfr.c               # Configuration File Region
├── die.c               # Error handling
├── Kconfig             # Kconfig configuration
└── acpi/               # Additional ACPI files
    ├── ec.asl
    ├── mainboard.asl
    └── superio.asl
```

## 📝 License

This project is **completely in the public domain** and can be used, modified, and distributed freely without any restrictions.

The original coreboot code is licensed under GPL-2.0.

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or pull requests.

## 📚 Resources

- [Coreboot Documentation](https://doc.coreboot.org/)
- [External Programmers Guide](https://www.flashrom.org/)

## ⚠️ Disclaimer

This firmware is provided "as is" without any warranty. Use this firmware at your own risk. Always make sure you have a backup of your original BIOS before proceeding.
