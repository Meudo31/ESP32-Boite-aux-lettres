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

// Niveau lu sur GPIO3 quand le clapet est OUVERT (le capteur reed relie GPIO3 à GND).
// 0 = contact fermé quand le clapet s'ouvre (recommandé : même capteur qu'aux étapes 2 et 3).
// 1 = contact fermé quand le clapet est fermé.
#define NIVEAU_CLAPET_OUVERT 0

// ---------- Bonus : copies de la notification (laisser vide pour désactiver) ----------
// E-mail : adresse qui reçoit une copie de chaque notification (transférée par ntfy)
#define EMAIL_ADRESSE      ""
// WhatsApp via CallMeBot : ton numéro au format international et la clé reçue (voir bonus/README.md)
#define WHATSAPP_TELEPHONE ""
#define WHATSAPP_CLE_API   ""
