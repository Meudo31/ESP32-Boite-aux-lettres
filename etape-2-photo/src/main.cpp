// Boîte aux lettres connectée - ESP32-CAM (AI-Thinker)
//
// Fonctionnement :
//  1. L'ESP32 dort (deep sleep) et se réveille quand le clapet s'ouvre (capteur reed sur GPIO13).
//  2. Il attend que le clapet se referme, laisse le courrier se poser.
//  3. Il prend une photo de l'intérieur avec le flash LED (GPIO4) à intensité réduite.
//  4. Il envoie la photo en notification sur le téléphone via ntfy, puis se rendort.
//
// Câblage du capteur reed : une patte sur GPIO13, l'autre sur GND.
// Même capteur qu'aux étapes 1 et 3 : contact ouvert clapet fermé (voir NIVEAU_CLAPET_OUVERT).

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include "driver/rtc_io.h"
#include "config.h"

// ---------- Broches ----------
#define BROCHE_REED   GPIO_NUM_13
#define BROCHE_LED    4
#define CANAL_LED     7   // canal LEDC libre (la caméra utilise le canal 0)

// Brochage caméra AI-Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Conservé pendant le deep sleep : vrai si on s'est endormi clapet ouvert
RTC_DATA_ATTR bool attenteFermeture = false;
RTC_DATA_ATTR uint32_t nbCourriers = 0;

// ---------- Clapet ----------
bool clapetOuvert() {
  return digitalRead(BROCHE_REED) == NIVEAU_CLAPET_OUVERT;
}

bool attendreFermeture(uint32_t delaiMaxMs) {
  uint32_t debut = millis();
  while (clapetOuvert()) {
    if (millis() - debut > delaiMaxMs) return false;
    delay(50);
  }
  return true;
}

// ---------- Caméra ----------
bool initCamera() {
  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;   // 800x600, suffisant pour lire une enveloppe
  config.jpeg_quality = 12;             // 10 = meilleure qualité, 63 = pire
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Erreur init caméra : 0x%x\n", err);
    return false;
  }
  return true;
}

bool ledAttachee = false;

void allumerLed(uint8_t intensite) {
  ledcSetup(CANAL_LED, 5000, 8);
  ledcAttachPin(BROCHE_LED, CANAL_LED);
  ledAttachee = true;
  ledcWrite(CANAL_LED, intensite);
}

void eteindreLed() {
  if (ledAttachee) {
    ledcWrite(CANAL_LED, 0);
    ledcDetachPin(BROCHE_LED);
    ledAttachee = false;
  }
  pinMode(BROCHE_LED, OUTPUT);
  digitalWrite(BROCHE_LED, LOW);
}

camera_fb_t* prendrePhoto() {
  allumerLed(LED_INTENSITE);
  // Quelques images jetées pour laisser l'exposition automatique s'adapter au flash
  for (int i = 0; i < 3; i++) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
    delay(150);
  }
  camera_fb_t* fb = esp_camera_fb_get();
  eteindreLed();
  if (!fb) Serial.println("Erreur : photo impossible");
  else Serial.printf("Photo prise : %u octets\n", fb->len);
  return fb;
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

// Envoie une notification ntfy, avec la photo en pièce jointe si fb n'est pas nul.
// Les en-têtes HTTP restent sans accents (ASCII obligatoire).
bool envoyerNtfy(const char* titre, const char* message, camera_fb_t* fb) {
  WiFiClientSecure client;
  client.setInsecure();  // pas de vérification du certificat : simple, suffisant pour ce projet
  HTTPClient http;
  String url = String(NTFY_SERVEUR) + "/" + NTFY_SUJET;
  if (!http.begin(client, url)) return false;

  http.addHeader("Title", titre);
  http.addHeader("Tags", "mailbox_with_mail");
  int code;
  if (fb) {
    http.addHeader("Filename", "courrier.jpg");
    http.addHeader("Message", message);
    code = http.sendRequest("PUT", fb->buf, fb->len);
  } else {
    code = http.POST((uint8_t*)message, strlen(message));
  }
  http.end();
  Serial.printf("Réponse ntfy : %d\n", code);
  return code == 200;
}

// ---------- Sommeil ----------
void allerDormir() {
  esp_camera_deinit();
  eteindreLed();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // LED éteinte et capteur photo coupé pendant tout le sommeil
  rtc_gpio_hold_en((gpio_num_t)BROCHE_LED);
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  rtc_gpio_hold_en((gpio_num_t)PWDN_GPIO_NUM);

  // Si le clapet est resté ouvert, on se réveillera à sa fermeture ; sinon à la prochaine ouverture
  attenteFermeture = clapetOuvert();
  int niveauReveil = attenteFermeture ? !NIVEAU_CLAPET_OUVERT : NIVEAU_CLAPET_OUVERT;
  rtc_gpio_pullup_en(BROCHE_REED);
  rtc_gpio_pulldown_dis(BROCHE_REED);
  esp_sleep_enable_ext0_wakeup(BROCHE_REED, niveauReveil);

  Serial.printf("Dodo (réveil quand le clapet %s)\n", attenteFermeture ? "se ferme" : "s'ouvre");
  Serial.flush();
  esp_deep_sleep_start();
}

// ---------- Programme principal ----------
void setup() {
  Serial.begin(115200);
  delay(100);

  rtc_gpio_hold_dis((gpio_num_t)BROCHE_LED);
  rtc_gpio_hold_dis((gpio_num_t)PWDN_GPIO_NUM);
  rtc_gpio_deinit(BROCHE_REED);
  pinMode(BROCHE_REED, INPUT_PULLUP);
  eteindreLed();

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

  if (cause != ESP_SLEEP_WAKEUP_EXT0) {
    // Premier démarrage (mise sous tension ou reset) : notification de test
    Serial.println("Démarrage de la boîte aux lettres");
    if (connecterWifi(15000)) {
      envoyerNtfy("Boite aux lettres", "Boite aux lettres en service", nullptr);
    }
    allerDormir();
  }

  Serial.println("Réveil : clapet actionné");
  if (!attenteFermeture && !attendreFermeture(ATTENTE_FERMETURE_MS)) {
    Serial.println("Clapet toujours ouvert, on attendra sa fermeture");
    allerDormir();
  }
  delay(DELAI_PHOTO_MS);

  if (!initCamera()) allerDormir();
  camera_fb_t* fb = prendrePhoto();

  if (connecterWifi(15000)) {
    nbCourriers++;
    char message[64];
    snprintf(message, sizeof(message), "Tu as du courrier ! (passage n.%lu)", (unsigned long)nbCourriers);
    if (!envoyerNtfy("Courrier", message, fb)) {
      // Repli : notification sans photo
      envoyerNtfy("Courrier", "Tu as du courrier ! (photo non envoyee)", nullptr);
    }
  }
  if (fb) esp_camera_fb_return(fb);
  allerDormir();
}

void loop() {
  // Jamais atteint : tout se passe dans setup() puis deep sleep
}
