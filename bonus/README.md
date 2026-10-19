# Bonus

Ces deux améliorations seront codées après les étapes 1 à 3. Voici comment on les fera.

## Second capteur (boîte à deux portes)

Pour une boîte avec un clapet à l'avant et une porte de relève à l'arrière.

- **Étape 1 (ESP32-C3)** : le second capteur va sur **GPIO4**, une autre broche capable de réveiller la carte. Le code saura quelle porte a été ouverte, par exemple pour distinguer « courrier déposé » de « courrier relevé ».
- **Étapes 2 et 3 (ESP32-CAM)** : il ne reste aucune broche libre fiable avec la carte SD (GPIO12 bloque le démarrage si elle est à HIGH). On branche donc les deux capteurs **en parallèle** sur GPIO13. Avec des contacts qui se ferment à l'ouverture, n'importe quelle porte réveille la carte, mais le code ne saura pas laquelle.

## Panneau solaire

Le but est de ne plus jamais recharger la batterie.

- Un panneau de **6 V et 1 à 2 W** se branche sur l'entrée IN+ / IN- du TP4056, qui accepte jusqu'à 8 V. C'est la solution la plus simple.
- Le module **CN3791** fait mieux, car il tire le maximum du panneau même par temps couvert. Il remplace le TP4056.
- Le panneau doit voir le ciel : une boîte à l'ombre toute la journée ne rechargera presque pas.
- **Attention au froid** : une batterie Li-ion ne doit pas être chargée en dessous de 0 °C, et le TP4056 ne le vérifie pas. Pour une boîte dehors en hiver, on ajoutera une mesure de température, ou bien on se contentera d'une batterie plus grosse.
