// Bonus : copie des notifications par e-mail et sur WhatsApp.
//
// Inclus par les trois étapes (chemin ajouté dans leur platformio.ini).
// Rien n'est envoyé tant que les réglages ci-dessous restent vides dans config.h.
//
//  - E-mail : le serveur ntfy transfère la notification à l'adresse indiquée
//    (en-tête "Email"). Avec une photo, l'e-mail contient le lien vers la photo.
//  - WhatsApp : message texte envoyé par le service gratuit CallMeBot
//    (pas de photo). Voir bonus/README.md pour obtenir la clé.
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#ifndef EMAIL_ADRESSE
#define EMAIL_ADRESSE ""
#endif
#ifndef WHATSAPP_TELEPHONE
#define WHATSAPP_TELEPHONE ""   // format international, ex. "+33612345678"
#endif
#ifndef WHATSAPP_CLE_API
#define WHATSAPP_CLE_API ""
#endif

// Ajoute la copie par e-mail à une requête ntfy en cours de préparation
inline void ajouterEmailNtfy(HTTPClient& http) {
  if (strlen(EMAIL_ADRESSE) > 0) http.addHeader("Email", EMAIL_ADRESSE);
}

// Encode un texte pour l'URL (UTF-8 compris)
inline String encoderUrl(const char* texte) {
  const char* hex = "0123456789ABCDEF";
  String sortie;
  for (const uint8_t* p = (const uint8_t*)texte; *p; p++) {
    if (isalnum(*p) || *p == '-' || *p == '_' || *p == '.' || *p == '~') {
      sortie += (char)*p;
    } else {
      sortie += '%';
      sortie += hex[*p >> 4];
      sortie += hex[*p & 0x0F];
    }
  }
  return sortie;
}

// Envoie le message sur WhatsApp via CallMeBot. Ne fait rien si non configuré.
inline bool envoyerWhatsApp(const char* message) {
  if (strlen(WHATSAPP_TELEPHONE) == 0 || strlen(WHATSAPP_CLE_API) == 0) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = String("https://api.callmebot.com/whatsapp.php?phone=") + encoderUrl(WHATSAPP_TELEPHONE) +
               "&text=" + encoderUrl(message) + "&apikey=" + encoderUrl(WHATSAPP_CLE_API);
  if (!http.begin(client, url)) return false;
  int code = http.GET();
  http.end();
  Serial.printf("Réponse WhatsApp : %d\n", code);
  return code == 200;
}
