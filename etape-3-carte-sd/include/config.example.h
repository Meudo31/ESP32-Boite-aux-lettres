// Copier ce fichier en config.h et remplir les valeurs.
// config.h ne doit jamais être publié (il contient le mot de passe Wi-Fi).
#pragma once

#define WIFI_SSID      "NomDeTonWifi"
#define WIFI_MOTDEPASSE "MotDePasseWifi"

// Sujet ntfy : n'importe qui connaissant ce nom peut lire tes notifications.
// Choisis un nom long et impossible à deviner, par exemple :
// bal-meudo-7f3k9q2x8w
#define NTFY_SUJET     "bal-change-moi-xxxxxxxx"
#define NTFY_SERVEUR   "https://ntfy.sh"

// Intensité du flash LED pendant la photo (0 à 255).
// La LED à fond surexpose à 10-20 cm : commencer vers 40-80.
#define LED_INTENSITE  60

// Délai entre la fermeture du clapet et la photo (ms), le temps que le courrier se pose.
#define DELAI_PHOTO_MS 1500

// Temps max d'attente de la fermeture du clapet avant de se rendormir (ms).
#define ATTENTE_FERMETURE_MS 30000

// Niveau lu sur GPIO13 quand le clapet est OUVERT (le capteur reed relie GPIO13 à GND).
// 0 = contact fermé quand le clapet s'ouvre (recommandé : obligatoire avec la carte SD,
//     car GPIO13 est la ligne D3 de la carte et doit rester à HIGH clapet fermé).
// 1 = contact fermé quand le clapet est fermé (uniquement sans carte SD).
#define NIVEAU_CLAPET_OUVERT 0

// Fuseau horaire pour dater les courriers (France, heure d'été automatique)
#define FUSEAU_HORAIRE "CET-1CEST,M3.5.0,M10.5.0/3"

// ---------- Bonus : copies de la notification (laisser vide pour désactiver) ----------
// E-mail : adresse qui reçoit une copie de chaque notification (transférée par ntfy)
#define EMAIL_ADRESSE      ""
// WhatsApp via CallMeBot : ton numéro au format international et la clé reçue (voir bonus/README.md)
#define WHATSAPP_TELEPHONE ""
#define WHATSAPP_CLE_API   ""
