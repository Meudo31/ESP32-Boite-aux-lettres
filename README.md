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

Prix approximatifs ; chaque étape réutilise ce qui a été acheté avant. Les liens ouvrent une recherche, car les fiches produits changent souvent : choisis un vendeur bien noté avec beaucoup de commandes. Amazon livre en 1 à 3 jours, AliExpress coûte environ moitié moins mais met 1 à 3 semaines. Achète la batterie 18650 en France ou chez NKON : celles d'AliExpress affichent souvent une fausse capacité.

### Étape 1 (environ 12 à 16 €)

| Pièce | Prix approx. | Où acheter |
| --- | --- | --- |
| ESP32-C3 SuperMini | 3 à 4 € | [Amazon](https://www.amazon.fr/s?k=ESP32-C3+SuperMini) · [AliExpress](https://www.aliexpress.com/w/wholesale-ESP32-C3-SuperMini.html) |
| Capteur reed + aimant (contact ouvert aimant proche, à vérifier au multimètre) | 1 à 2 € | [Amazon](https://www.amazon.fr/s?k=MC-38+capteur+magn%C3%A9tique) · [AliExpress](https://www.aliexpress.com/w/wholesale-MC-38-reed-switch.html) |
| Batterie 18650 de marque + support | 4 à 6 € | [Amazon](https://www.amazon.fr/s?k=batterie+18650+Samsung+25R) · [support](https://www.amazon.fr/s?k=support+batterie+18650+fils) · [NKON](https://www.nkon.nl/fr/) |
| Module de charge TP4056 avec protection (6 broches, USB-C) | 1 € | [Amazon](https://www.amazon.fr/s?k=TP4056+USB-C+protection) · [AliExpress](https://www.aliexpress.com/w/wholesale-TP4056-type-c-protection.html) |
| Régulateur HT7333 (ou MCP1700-3302) | 1 € | [Amazon](https://www.amazon.fr/s?k=HT7333) · [AliExpress](https://www.aliexpress.com/w/wholesale-HT7333.html) |
| Fils Dupont, petit boîtier étanche | 2 à 4 € | [Amazon](https://www.amazon.fr/s?k=fils+dupont) · [boîtier](https://www.amazon.fr/s?k=boitier+%C3%A9tanche+%C3%A9lectronique+IP65+petit) · [AliExpress](https://www.aliexpress.com/w/wholesale-dupont-wire.html) |

### Étape 2 (environ 8 à 12 € en plus)

| Pièce | Prix approx. | Où acheter |
| --- | --- | --- |
| ESP32-CAM AI-Thinker (caméra OV2640) | 6 à 8 € | [Amazon](https://www.amazon.fr/s?k=ESP32-CAM+AI-Thinker+OV2640) · [AliExpress](https://www.aliexpress.com/w/wholesale-ESP32-CAM-AI-Thinker.html) |
| Carte de programmation ESP32-CAM-MB | 2 à 4 € | [Amazon](https://www.amazon.fr/s?k=ESP32-CAM-MB) · [AliExpress](https://www.aliexpress.com/w/wholesale-ESP32-CAM-MB.html) |
| Condensateur 470 à 1000 µF, 6,3 V ou plus | moins d'1 € | [Amazon](https://www.amazon.fr/s?k=condensateur+1000uF+10V) · [AliExpress](https://www.aliexpress.com/w/wholesale-1000uF-10V-capacitor.html) |

### Étape 3 (environ 5 € en plus)

| Pièce | Prix approx. | Où acheter |
| --- | --- | --- |
| Carte micro SD de 32 Go maximum, formatée en FAT32 | 5 € | [Amazon](https://www.amazon.fr/s?k=carte+micro+SD+32+Go+SanDisk) · en magasin aussi |

### Bonus

| Pièce | Prix approx. | Où acheter |
| --- | --- | --- |
| Second capteur reed + aimant | 1 à 2 € | [Amazon](https://www.amazon.fr/s?k=MC-38+capteur+magn%C3%A9tique) · [AliExpress](https://www.aliexpress.com/w/wholesale-MC-38-reed-switch.html) |
| Panneau solaire 6 V, 1 à 2 W | 5 à 10 € | [Amazon](https://www.amazon.fr/s?k=panneau+solaire+6V+2W) · [AliExpress](https://www.aliexpress.com/w/wholesale-solar-panel-6V-2W.html) |
| Module de charge solaire CN3791 (à la place du TP4056, optionnel) | 2 à 4 € | [Amazon](https://www.amazon.fr/s?k=CN3791) · [AliExpress](https://www.aliexpress.com/w/wholesale-CN3791.html) |

## Mise en route

Chaque dossier a son propre README avec le câblage et la mise en route. Dans tous les cas : copier `include/config.example.h` en `include/config.h`, remplir le Wi-Fi et le sujet ntfy, puis `pio run -t upload` depuis le dossier de l'étape. Le fichier `config.h` est exclu du dépôt.
