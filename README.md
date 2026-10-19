# Boîte aux lettres connectée

Une boîte aux lettres qui prévient sur le téléphone quand le facteur passe. Le projet avance par étapes, du plus simple au plus complet ; chaque étape est un projet PlatformIO autonome et peut faire l'objet d'une vidéo.

| Étape | Dossier | Ce que ça fait | Carte |
| --- | --- | --- | --- |
| 1 | [etape-1-notification](etape-1-notification) | Notification « Tu as du courrier ! » à l'ouverture du clapet | ESP32-C3 SuperMini |
| 2 | [etape-2-photo](etape-2-photo) | Photo du courrier au flash LED, envoyée sur le téléphone | ESP32-CAM |
| 3 | [etape-3-carte-sd](etape-3-carte-sd) | Archive des photos et journal daté des dépôts sur carte micro SD | ESP32-CAM |
| Bonus | [bonus](bonus) | Second capteur pour les boîtes à deux portes, panneau solaire | |

Les trois étapes utilisent le même capteur reed monté dans le même sens : contact ouvert quand le clapet est fermé. Ce sens est obligatoire à l'étape 3, autant le choisir dès le début.

## Liste d'achat par étape

Prix approximatifs ; chaque étape réutilise ce qui a été acheté avant.

### Étape 1 (environ 12 à 16 €)

| Pièce | Prix approx. |
| --- | --- |
| ESP32-C3 SuperMini | 3 à 4 € |
| Capteur reed + aimant (contact ouvert aimant proche, à vérifier au multimètre) | 1 à 2 € |
| Batterie 18650 de marque + support | 4 à 6 € |
| Module de charge TP4056 avec protection (6 broches, USB-C) | 1 € |
| Régulateur HT7333 (ou MCP1700-3302) | 1 € |
| Fils Dupont, petit boîtier étanche | 2 à 4 € |

### Étape 2 (environ 8 à 12 € en plus)

| Pièce | Prix approx. |
| --- | --- |
| ESP32-CAM AI-Thinker (caméra OV2640) | 6 à 8 € |
| Carte de programmation ESP32-CAM-MB | 2 à 4 € |
| Condensateur 470 à 1000 µF, 6,3 V ou plus | moins d'1 € |

### Étape 3 (environ 5 € en plus)

| Pièce | Prix approx. |
| --- | --- |
| Carte micro SD de 32 Go maximum, formatée en FAT32 | 5 € |

### Bonus

| Pièce | Prix approx. |
| --- | --- |
| Second capteur reed + aimant | 1 à 2 € |
| Panneau solaire 6 V, 1 à 2 W | 5 à 10 € |
| Module de charge solaire CN3791 (à la place du TP4056, optionnel) | 2 à 4 € |

## Mise en route

Chaque dossier a son propre README avec le câblage et la mise en route. Dans tous les cas : copier `include/config.example.h` en `include/config.h`, remplir le Wi-Fi et le sujet ntfy, puis `pio run -t upload` depuis le dossier de l'étape. Le fichier `config.h` est exclu du dépôt.
