// Boîte aux lettres connectée - étape 1 : ESP32-C3 SuperMini, sans caméra
//
// Fonctionnement :
//  1. L'ESP32-C3 dort (deep sleep) et se réveille quand le clapet s'ouvre (capteur reed sur GPIO3).
//  2. Il se connecte au Wi-Fi et envoie une notification sur le téléphone via ntfy.
//  3. Il se rendort. Si le clapet est resté ouvert, il attend sa fermeture sans renvoyer de notification.
//
// Câblage du capteur reed : une patte sur GPIO3, l'autre sur GND.
// Sur l'ESP32-C3, seules GPIO0 à GPIO5 peuvent réveiller la carte.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "driver/gpio.h"
#include "config.h"
#include "bonus_notifications.h"

#define BROCHE_REED   GPIO_NUM_3

// Conservé pendant le deep sleep
RTC_DATA_ATTR bool attenteFermeture = false;  // vrai si on s'est endormi clapet ouvert
RTC_DATA_ATTR uint32_t nbCourriers = 0;

// ---------- Clapet ----------
bool clapetOuvert() {
  return digitalRead(BROCHE_REED) == NIVEAU_CLAPET_OUVERT;
}

// ---------- Réseau ----------
bool connecterWifi(uint32_t delaiMaxMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_MOTDEPASSE);
  uint32_t debut = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - debut > delaiMaxMs) {
      Serial.println("Erreur : Wi-Fi injoignable");
      return false;
    }
    delay(100);
  }
  Serial.printf("Wi-Fi connecté, signal %d dBm\n", WiFi.RSSI());
  return true;
}

// Envoie une notification texte ntfy. Les en-têtes HTTP restent sans accents (ASCII obligatoire).
bool envoyerNtfy(const char* titre, const char* message) {
  WiFiClientSecure client;
  client.setInsecure();  // pas de vérification du certificat : simple, suffisant pour ce projet
  HTTPClient http;
  String url = String(NTFY_SERVEUR) + "/" + NTFY_SUJET;
  if (!http.begin(client, url)) return false;
  http.addHeader("Title", titre);
  http.addHeader("Tags", "mailbox_with_mail");
  ajouterEmailNtfy(http);
  int code = http.POST((uint8_t*)message, strlen(message));
  http.end();
  envoyerWhatsApp(message);
  Serial.printf("Réponse ntfy : %d\n", code);
  return code == 200;
}

// ---------- Sommeil ----------
void allerDormir() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Si le clapet est resté ouvert, on se réveillera à sa fermeture ; sinon à la prochaine ouverture
  attenteFermeture = clapetOuvert();
  int niveauReveil = attenteFermeture ? !NIVEAU_CLAPET_OUVERT : NIVEAU_CLAPET_OUVERT;

  // Résistance de tirage gardée pendant le sommeil
  gpio_pullup_en(BROCHE_REED);
  gpio_pulldown_dis(BROCHE_REED);
  gpio_hold_en(BROCHE_REED);
  gpio_deep_sleep_hold_en();
  esp_deep_sleep_enable_gpio_wakeup(1ULL << BROCHE_REED,
      niveauReveil ? ESP_GPIO_WAKEUP_GPIO_HIGH : ESP_GPIO_WAKEUP_GPIO_LOW);

  Serial.printf("Dodo (réveil quand le clapet %s)\n", attenteFermeture ? "se ferme" : "s'ouvre");
  Serial.flush();
  esp_deep_sleep_start();
}

// ---------- Programme principal ----------
void setup() {
  Serial.begin(115200);
  delay(200);

  gpio_hold_dis(BROCHE_REED);
  pinMode(BROCHE_REED, INPUT_PULLUP);

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

  if (cause != ESP_SLEEP_WAKEUP_GPIO) {
    // Premier démarrage (mise sous tension ou reset) : notification de test
    Serial.println("Démarrage de la boîte aux lettres");
    if (connecterWifi(15000)) envoyerNtfy("Boite aux lettres", "Boite aux lettres en service");
    allerDormir();
  }

  if (attenteFermeture) {
    // Réveil dû à la fermeture du clapet : déjà notifié à l'ouverture
    Serial.println("Clapet refermé");
    allerDormir();
  }

  Serial.println("Réveil : clapet ouvert");
  nbCourriers++;
  if (connecterWifi(15000)) {
    char message[64];
    snprintf(message, sizeof(message), "Tu as du courrier ! (passage n.%lu)", (unsigned long)nbCourriers);
    envoyerNtfy("Courrier", message);
  }
  delay(2000);  // anti-rebond : laisse le clapet retomber avant de choisir le prochain réveil
  allerDormir();
}

void loop() {
  // Jamais atteint : tout se passe dans setup() puis deep sleep
}
