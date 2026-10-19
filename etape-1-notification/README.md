# Étape 1 : la notification (ESP32-C3, sans caméra)

Quand le facteur ouvre le clapet, l'ESP32-C3 se réveille et envoie « Tu as du courrier ! » sur ton téléphone via l'appli **ntfy**. Il se rendort ensuite jusqu'au passage suivant.

Le code compile avec PlatformIO (ESP32-C3 SuperMini, framework Arduino) mais n'a pas encore été testé sur une vraie carte.

## Câblage

| De | Vers |
| --- | --- |
| Capteur reed, patte 1 | GPIO3 |
| Capteur reed, patte 2 | GND |
| Sortie OUT+ du TP4056 (batterie +) | Entrée du HT7333 |
| Sortie 3,3 V du HT7333 | Broche **3V3** de l'ESP32-C3 |
| OUT- du TP4056 (batterie -) | GND |

Place l'aimant sur le clapet et le reed sur le cadre. **Le contact doit être ouvert quand le clapet est fermé** et se fermer quand le clapet s'ouvre : c'est le sens qui marchera aussi aux étapes 2 et 3 avec la carte SD. Vérifie ton capteur au multimètre, aimant collé puis éloigné. S'il fait l'inverse, mets `NIVEAU_CLAPET_OUVERT` à 1 dans `config.h` pour cette étape.

Pendant les essais, tu peux alimenter la carte par son port USB-C au lieu de la batterie.

## Mise en route

1. Installe l'appli **ntfy** et abonne-toi à un sujet au nom long et impossible à deviner.
2. Copie `include/config.example.h` en `include/config.h`, puis remplis le Wi-Fi et le sujet ntfy.
3. Branche la carte en USB-C et lance `pio run -t upload`, puis `pio device monitor`. Si le téléversement échoue, maintiens le bouton BOOT en branchant la carte.
4. Au démarrage, tu reçois « Boite aux lettres en service ». Ouvre le contact (éloigne ou approche l'aimant selon ton capteur) : la notification arrive quelques secondes plus tard.

## Points à vérifier ensemble

- **Réveil** : le code garde la résistance de tirage interne pendant le sommeil. Si la carte ne se réveille pas à l'ouverture, ajoute une résistance de 100 kΩ entre GPIO3 et 3V3.
- **Moniteur série** : après un réveil, l'USB met un instant à revenir, donc les premières lignes peuvent manquer. C'est normal.
- **Consommation en sommeil** : l'ESP32-C3 SuperMini consomme beaucoup moins que l'ESP32-CAM. C'est à mesurer au multimètre, ce qui donnera un point de comparaison pour l'étape 2.
