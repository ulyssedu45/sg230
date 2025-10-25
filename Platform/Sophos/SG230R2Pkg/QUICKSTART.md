# Quick Start Guide - Sophos SG230 Rev 2 UEFI EDK2

## 🚀 Installation Rapide (5 minutes)

### Prérequis
- Ubuntu 20.04+ ou Debian 11+
- 8GB RAM minimum
- 20GB espace disque libre
- Git, GCC installés

### Installation en Une Commande

```bash
# Installer tout automatiquement
curl -fsSL https://raw.githubusercontent.com/ulyssedu45/sg230/main/Platform/Sophos/SG230R2Pkg/quick_setup.sh | bash
```

### Installation Manuelle

```bash
# 1. Installer les dépendances
sudo apt update
sudo apt install -y build-essential uuid-dev iasl git nasm python3 \
                    python3-pip gcc-multilib g++-multilib acpica-tools

# 2. Cloner EDK2
cd ~
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init

# 3. Ajouter la plateforme SG230
git clone https://github.com/ulyssedu45/sg230.git
ln -s ~/sg230/Platform/Sophos ~/edk2/Platform/

# 4. Compiler
cd ~/edk2
source edksetup.sh
make -C BaseTools
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

### Résultat

Le firmware sera créé dans:
```
~/edk2/Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd
```

## 🔧 Flasher le Firmware

### ⚠️ IMPORTANT: Sauvegarder d'abord!

```bash
# Sauvegarder le firmware actuel
sudo flashrom -p <votre_programmeur> -r backup_$(date +%Y%m%d).bin

# Vérifier la sauvegarde
sudo flashrom -p <votre_programmeur> -v backup_$(date +%Y%m%d).bin
```

### Créer l'Image Finale

```bash
cd ~/sg230/Platform/Sophos/SG230R2Pkg/
./create_flash_image.sh ~/edk2/Build/SG230R2/RELEASE_GCC5/FV/SG230R2.fd backup_$(date +%Y%m%d).bin
```

### Flasher

```bash
sudo flashrom -p <votre_programmeur> -w sg230r2_final.bin
```

## 📊 Vérification

### Console Série

Connectez-vous à COM1 (115200 8N1):
```bash
sudo screen /dev/ttyUSB0 115200
```

Vous devriez voir:
```
UEFI v2.8 (SG230R2 v0.1)
Memory: XXX MB
Initializing devices...
```

## ❓ Problèmes Courants

### "Build failed" lors de la compilation

```bash
cd ~/edk2
make -C BaseTools clean
make -C BaseTools
build clean
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
```

### "No video output" au démarrage

1. Connectez-vous via série (COM1)
2. Vérifiez que l'IGPU Intel est activé
3. Testez avec un autre câble/moniteur

### "System won't boot"

1. Vérifiez la console série pour les erreurs
2. Assurez-vous que la région Intel ME est intacte
3. Testez avec configuration minimale (1 barrette RAM)

## 📚 Documentation Complète

- **Setup détaillé**: [README.md](README.md) ou [README.fr.md](README.fr.md)
- **Guide de conversion**: [CONVERSION_GUIDE.md](CONVERSION_GUIDE.md)
- **Vue d'ensemble**: [../../../README_EDK2.md](../../../README_EDK2.md)

## 🆘 Support

- **Issues**: https://github.com/ulyssedu45/sg230/issues
- **Discord**: [Lien Discord si disponible]
- **Forum**: [Forum communautaire si disponible]

## ✅ Checklist Post-Installation

Après le premier boot:

- [ ] Vérifier la détection RAM
- [ ] Tester USB (clavier/souris)
- [ ] Vérifier SATA/disques
- [ ] Tester le boot depuis USB
- [ ] Configurer le boot order
- [ ] Tester le réseau (PXE)
- [ ] Sauvegarder les paramètres UEFI

## 🎯 Prochaines Étapes

1. **Installer un OS**: Windows 10/11, Linux, FreeBSD
2. **Configurer UEFI Setup**: Appuyez sur F2/DEL au boot
3. **Optimiser les paramètres**: CPU, RAM, ventilateurs
4. **Explorer UEFI Shell**: Commandes utiles

## 💡 Astuces

### Accès UEFI Setup
- Appuyez sur **F2** ou **DEL** au démarrage
- Depuis l'OS: `systemctl reboot --firmware-setup` (Linux)

### Boot depuis USB
1. Créez une clé USB UEFI bootable
2. Appuyez sur **F12** pour le boot menu
3. Sélectionnez votre clé USB

### Variables UEFI persistantes
Les variables sont stockées dans la flash et persistent entre les redémarrages.

## 🔄 Mises à Jour

Pour mettre à jour le firmware:

```bash
cd ~/edk2
git pull
cd ~/sg230
git pull
./Platform/Sophos/SG230R2Pkg/build.sh RELEASE GCC5
# Suivre la procédure de flash
```

## 📝 Notes

- Ce firmware est **compatible UEFI 2.8**
- **Secure Boot** peut être activé ultérieurement
- **Compatible**: Windows 8+, Linux (kernel 3.0+), FreeBSD 11+
- **CPU supportés**: Intel 6ème-9ème gen (Skylake, Kaby Lake, Coffee Lake)

---

**Temps total estimé**: 15-30 minutes (compilation + flash)

**Niveau de difficulté**: Intermédiaire (besoin d'un programmeur SPI)

**Risque**: Moyen (possible brick si mal fait - gardez toujours une sauvegarde!)
