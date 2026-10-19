// Boîte aux lettres connectée - ESP32-CAM (AI-Thinker)
//
// Fonctionnement :
//  1. L'ESP32 dort (deep sleep) et se réveille quand le clapet s'ouvre (capteur reed sur GPIO13).
//  2. Il attend que le clapet se referme, laisse le courrier se poser.
//  3. Il prend une photo de l'intérieur avec le flash LED (GPIO4) à intensité réduite.
//  4. Il se connecte au Wi-Fi et met l'heure à jour (NTP).
//  5. Il enregistre la photo et une ligne de journal datée sur la carte micro SD.
//  6. Il envoie la photo en notification sur le téléphone via ntfy, puis se rendort.
//
// Carte SD en mode 1 bit : seules GPIO2, 14 et 15 servent à la carte,
// ce qui laisse GPIO4 (LED) et GPIO13 (reed) libres.
//
// Câblage du capteur reed : une patte sur GPIO13, l'autre sur GND.
// GPIO13 est aussi la ligne D3 de la carte SD : elle doit être à HIGH quand la carte
// démarre, donc le contact doit être OUVERT quand le clapet est fermé
// (voir NIVEAU_CLAPET_OUVERT dans config.h et le README).

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SD_MMC.h>
#include <time.h>
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

#define DOSSIER_SD   "/courrier"
#define JOURNAL_SD   "/courrier/journal.csv"

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

// Heure valide = synchronisée au moins une fois (l'horloge interne continue pendant le sommeil)
bool heureValide() {
  return time(nullptr) > 1700000000;  // après novembre 2023
}

void synchroniserHeure(uint32_t delaiMaxMs) {
  configTzTime(FUSEAU_HORAIRE, "pool.ntp.org", "time.google.com");
  uint32_t debut = millis();
  struct tm t;
  while (!getLocalTime(&t, 100)) {
    if (millis() - debut > delaiMaxMs) {
      Serial.println("Attention : heure NTP non reçue");
      return;
    }
  }
  Serial.printf("Heure : %02d/%02d/%04d %02d:%02d:%02d\n",
                t.tm_mday, t.tm_mon + 1, t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);
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

// ---------- Carte SD ----------
bool initSD() {
  // true = mode 1 bit : libère GPIO4 (LED) et GPIO13 (reed)
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Erreur : carte SD absente ou illisible");
    return false;
  }
  if (!SD_MMC.exists(DOSSIER_SD)) SD_MMC.mkdir(DOSSIER_SD);
  if (!SD_MMC.exists(JOURNAL_SD)) {
    File f = SD_MMC.open(JOURNAL_SD, FILE_WRITE);
    if (f) {
      f.println("numero;date;heure;photo;taille_octets;notification");
      f.close();
    }
  }
  return true;
}

// Construit le nom de la photo à partir de la date, ou du numéro si l'heure est inconnue
String nomPhoto(const struct tm& t, bool avecDate) {
  char nom[48];
  if (avecDate) {
    snprintf(nom, sizeof(nom), DOSSIER_SD "/%04d-%02d-%02d_%02d-%02d-%02d.jpg",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  } else {
    snprintf(nom, sizeof(nom), DOSSIER_SD "/sans-date_%05lu.jpg", (unsigned long)nbCourriers);
  }
  return String(nom);
}

String enregistrerPhoto(camera_fb_t* fb, const struct tm& t, bool avecDate) {
  String nom = nomPhoto(t, avecDate);
  File f = SD_MMC.open(nom, FILE_WRITE);
  if (!f) {
    Serial.println("Erreur : écriture de la photo impossible");
    return "";
  }
  f.write(fb->buf, fb->len);
  f.close();
  Serial.printf("Photo enregistrée : %s\n", nom.c_str());
  return nom;
}

void ecrireJournal(const struct tm& t, bool avecDate, const String& photo, size_t taille, bool notifOk) {
  File f = SD_MMC.open(JOURNAL_SD, FILE_APPEND);
  if (!f) {
    Serial.println("Erreur : écriture du journal impossible");
    return;
  }
  char date[16] = "inconnue";
  char heure[16] = "inconnue";
  if (avecDate) {
    strftime(date, sizeof(date), "%d/%m/%Y", &t);
    strftime(heure, sizeof(heure), "%H:%M:%S", &t);
  }
  f.printf("%lu;%s;%s;%s;%u;%s\n", (unsigned long)nbCourriers, date, heure,
           photo.length() ? photo.c_str() : "aucune", (unsigned)taille, notifOk ? "envoyee" : "echec");
  f.close();
}

// ---------- Sommeil ----------
void allerDormir() {
  SD_MMC.end();
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

  setenv("TZ", FUSEAU_HORAIRE, 1);
  tzset();
  rtc_gpio_hold_dis((gpio_num_t)BROCHE_LED);
  rtc_gpio_hold_dis((gpio_num_t)PWDN_GPIO_NUM);
  rtc_gpio_deinit(BROCHE_REED);
  pinMode(BROCHE_REED, INPUT_PULLUP);
  eteindreLed();

  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

  if (cause != ESP_SLEEP_WAKEUP_EXT0) {
    // Premier démarrage (mise sous tension ou reset) : test de la carte SD et notification
    Serial.println("Démarrage de la boîte aux lettres");
    bool sdOk = initSD();
    if (connecterWifi(15000)) {
      synchroniserHeure(5000);
      envoyerNtfy("Boite aux lettres",
                  sdOk ? "Boite aux lettres en service (carte SD OK)"
                       : "Boite aux lettres en service (carte SD absente)", nullptr);
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
  nbCourriers++;

  bool wifiOk = connecterWifi(15000);
  if (wifiOk) synchroniserHeure(5000);

  // Heure du dépôt : NTP si dispo, sinon horloge interne (synchronisée au dernier passage)
  struct tm t = {};
  bool avecDate = heureValide();
  if (avecDate) getLocalTime(&t, 0);

  bool sdOk = initSD();
  String photo = "";
  if (sdOk && fb) photo = enregistrerPhoto(fb, t, avecDate);

  bool notifOk = false;
  if (wifiOk) {
    char message[96];
    if (avecDate) {
      snprintf(message, sizeof(message), "Tu as du courrier ! (passage n.%lu a %02d:%02d)",
               (unsigned long)nbCourriers, t.tm_hour, t.tm_min);
    } else {
      snprintf(message, sizeof(message), "Tu as du courrier ! (passage n.%lu)", (unsigned long)nbCourriers);
    }
    notifOk = envoyerNtfy("Courrier", message, fb);
    if (!notifOk) {
      // Repli : notification sans photo
      notifOk = envoyerNtfy("Courrier", "Tu as du courrier ! (photo non envoyee)", nullptr);
    }
  }

  if (sdOk) ecrireJournal(t, avecDate, photo, fb ? fb->len : 0, notifOk);
  if (fb) esp_camera_fb_return(fb);
  allerDormir();
}

void loop() {
  // Jamais atteint : tout se passe dans setup() puis deep sleep
}
