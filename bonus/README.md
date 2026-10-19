# Bonus

Le second capteur et le panneau solaire seront codés après les étapes 1 à 3 ; les notifications par e-mail et WhatsApp sont déjà prêtes.

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

## Notifications par e-mail et sur WhatsApp

Le code est prêt dans les trois étapes, mais rien n'est envoyé tant que les réglages restent vides dans `config.h`. Ces réglages se trouvent en bas de `config.example.h`, et le code commun est dans [notifications/bonus_notifications.h](notifications/bonus_notifications.h).

### E-mail

Mets ton adresse dans `EMAIL_ADRESSE`. C'est le serveur ntfy qui transfère chaque notification par e-mail : aucun mot de passe de messagerie n'est stocké dans l'ESP32. Avec une photo (étapes 2 et 3), l'e-mail contient un lien vers la photo plutôt que la photo elle-même.

Le serveur public ntfy.sh limite le nombre d'e-mails par jour (une quinzaine), ce qui suffit pour une boîte aux lettres. À vérifier au premier essai : si l'e-mail n'arrive pas, regarde le dossier spam, puis la réponse ntfy dans le moniteur série.

### WhatsApp

Le message arrive en texte, sans la photo. Il passe par CallMeBot, un service gratuit pour usage personnel.

1. Ajoute le numéro de CallMeBot dans tes contacts. Il est indiqué sur [callmebot.com](https://www.callmebot.com/blog/free-api-whatsapp-messages/), car il change parfois.
2. Envoie-lui sur WhatsApp le message « I allow callmebot to send me messages ».
3. Tu reçois en retour une clé (apikey). Mets-la dans `WHATSAPP_CLE_API`, et ton numéro au format international (par exemple `+33612345678`) dans `WHATSAPP_TELEPHONE`.

Le message WhatsApp part en même temps que la notification ntfy, y compris le message « en service » au démarrage, ce qui permet de tester.
