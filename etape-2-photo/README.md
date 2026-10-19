# Étape 2 : la photo du courrier (ESP32-CAM)

Quand le facteur ouvre le clapet, l'ESP32-CAM se réveille. Il attend que le clapet se referme, prend une photo du courrier avec la LED en flash, et l'envoie sur ton téléphone via l'appli **ntfy**. Il se rendort ensuite jusqu'au passage suivant.

Le code compile avec PlatformIO (ESP32-CAM AI-Thinker, framework Arduino) mais n'a pas encore été testé sur une vraie carte.

## Matériel

| Pièce | Rôle | Prix approx. |
| --- | --- | --- |
| ESP32-CAM AI-Thinker (caméra OV2640) | Cerveau + caméra + LED flash | 6 à 8 € |
| Carte de programmation ESP32-CAM-MB (USB) ou adaptateur FTDI 3,3 V | Flasher la carte, qui n'a pas d'USB | 2 à 4 € |
| Capteur reed + aimant de l'étape 1 | Détecter l'ouverture du clapet | déjà acheté |
| Batterie 18650 + support | Alimentation | 4 à 6 € |
| Module de charge TP4056 (avec protection) | Recharger la batterie | 1 € |
| Régulateur faible chute HT7333 (ou MCP1700-3302) | Sortir 3,3 V propres de la batterie | 1 € |

## Câblage

| De | Vers |
| --- | --- |
| Capteur reed, patte 1 | GPIO13 |
| Capteur reed, patte 2 | GND |
| Batterie + (sortie TP4056 OUT+) | Entrée du HT7333 |
| Sortie 3,3 V du HT7333 | Broche **3V3** de l'ESP32-CAM |
| Batterie - (OUT-) | GND |

Même capteur et même placement qu'à l'étape 1 : contact ouvert quand le clapet est fermé, fermé quand il s'ouvre (`NIVEAU_CLAPET_OUVERT` à 0). C'est ce sens qui permettra d'ajouter la carte SD à l'étape 3.

On alimente par la broche 3V3 plutôt que 5V : le régulateur 5 V intégré à la carte consomme en permanence et viderait la batterie. Prévois un condensateur de 470 à 1000 µF entre 3V3 et GND : le Wi-Fi tire des pointes de courant qui font souvent redémarrer l'ESP32-CAM (message « Brownout detector was triggered »).

## Mise en route

1. Installe l'appli **ntfy** sur ton téléphone et abonne-toi à un sujet au nom long et impossible à deviner (exemple : `bal-meudo-7f3k9q2x8w`). Sur le serveur public ntfy.sh, n'importe qui connaissant ce nom peut voir les photos.
2. Copie `include/config.example.h` en `include/config.h`, puis remplis le Wi-Fi et le sujet ntfy.
3. Branche la carte sur l'ESP32-CAM-MB et lance `pio run -t upload`, puis `pio device monitor`. Avec un adaptateur FTDI, relie GPIO0 à GND pendant le flash, puis retire le fil et fais un reset.
4. Au démarrage, tu dois recevoir « Boite aux lettres en service ». Ouvre et referme le contact reed (ou approche puis éloigne l'aimant) : la photo arrive quelques secondes plus tard.

## Réglages (`config.h`)

- `LED_INTENSITE` (0 à 255) : la LED à fond surexpose de près. Commence à 60 et ajuste selon la photo.
- `DELAI_PHOTO_MS` : délai entre la fermeture du clapet et la photo.
- `ATTENTE_FERMETURE_MS` : si le clapet reste ouvert plus longtemps que ce délai, la carte se rendort et prend la photo à sa fermeture.

## Points à vérifier ensemble

- **Portée Wi-Fi** jusqu'à la boîte : le moniteur série affiche le signal en dBm. En dessous de -80 dBm, la connexion sera instable.
- **Consommation en sommeil** : sur une ESP32-CAM, compte environ 1 mA ou plus selon la carte, soit quelques semaines à deux mois sur une 18650. C'est à mesurer au multimètre, et ça fera un bon sujet de Short.
- **Pièces jointes ntfy.sh** : sur le serveur public, elles expirent au bout de quelques heures. L'appli les télécharge sur le téléphone à la réception.

## Ce qui se passe à chaque passage (moniteur série)

```
Réveil : clapet actionné
Photo prise : 41236 octets
Wi-Fi connecté, signal -67 dBm
Réponse ntfy : 200
Dodo (réveil quand le clapet s'ouvre)
```
