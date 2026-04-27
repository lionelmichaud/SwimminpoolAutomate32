# Changelog — ESP32_Automat (Automatisme piscine)

Toutes les modifications notables sont documentées dans ce fichier.

---

## [2.4.0] — 2026-04-27 (en cours)

Compatible AsyncTCP v3.4.10 ·ESPAsyncWebServer v3.1.0 · ESP32 v3.3.8 · ESP8266 v3.1.2

### Fiabilisation des mesures de température Dallas (1-Wire)

- Précision réduite de **11 → 10 bits** (`TEMPERATURE_PRECISION`) pour accélérer la conversion
- Délai de mesure augmenté de **+100 ms** (`delaySamplingTemp`) pour laisser le temps aux capteurs
- Nouvelle fonction **`readTempRaw(deviceIndex)`** : lit la température avec un retry automatique si la valeur retournée est -127 °C (erreur CRC 1-Wire) ou 85 °C (valeur de mise sous tension parasite)
- Nouvelle fonction **`isSensorInitOK(deviceID)`** : vérifie si un capteur a été initialisé sans erreur avant de le lire
- `setResolution()` conditionné à l'absence d'erreur d'init pour chaque capteur — évite d'envoyer une trame invalide qui perturberait les autres capteurs du bus
- `SampleTemperatures()` : déclenchement de la lecture rendu **indépendant du flag global** `ErrorTempSensorInit` — les capteurs fonctionnels continuent d'être lus même si l'un d'eux a échoué à l'initialisation
- `AcquireTemperatures()` et `InitTemperatureSensors()` : lectures conditionnées capteur par capteur via `isSensorInitOK()`
- `InitTemperatureSensors()` : `delay(2000)` déplacé **après** `DisplayAlert()` (affichage d'abord, pause ensuite)

### Robustesse du bus I2C

- Bus I2C initialisé avec **timeout de 3 ms** (`Wire.setTimeOut(3)`) dans `initializeOLED()`
- Nouvelle fonction **`recoverI2C()`** : récupération du bus I2C bloqué
  - Envoie 9 impulsions d'horloge sur SCL pour libérer un SDA figé
  - Réinitialise `Wire` et le display OLED
  - Appelée automatiquement dans `loop()` si `ui.update()` retourne une valeur < -500

### Affichage OLED

- Fréquence OLED réduite de **25 fps → 20 fps**
- Nouvelle frame **`drawWaterTemperatures()`** : affiche la température de l'eau en grand avec la police `Dialog_bold_32` (nouveau fichier `font.h`)
- En mode non-debug : `drawDeviceInfoTemperatures` remplacée par `drawWaterTemperatures`
- Remplacement des concaténations `String +` par `snprintf()` + buffer pour éviter les allocations dynamiques

### Domotique

- Nouvel endpoint **`GET /statusJSON`** : retourne l'état complet de l'automate au format JSON
  ```json
  {
    "waterTemp": "24.3",
    "airTemp": "28.1",
    "internalTemp": "31.0",
    "mode": "automatique",
    "cover": "ouvert"
  }
  ```
- Migration **ArduinoJson v5 → v7** dans `JSON.ino` (`StaticJsonBuffer` → `JsonDocument`, `parseObject` → `deserializeJson`)

### Divers

- Période d'exécution de la task automate : **500 ms → 1 s** (`delay(1000)`)
- Ajout de la commande `/info` dans la page de texte d'information du serveur Web
- Ajout du bitmap **`alert_bits`** (16×16) dans `images.h`
- `Automat.ino` : reformatage du code (style K&R, espacements) — aucun changement fonctionnel

---

## [2.3.2]

Compatible AsyncTCP v1.0.0 · ESP32 v1.0.0

- Inversion de la logique du switch Manuel / Automatique

---

## [2.3.1]

Compatible AsyncTCP v1.0.0 · ESP32 v1.0.0

- Inversion de l'affichage OLED (collé dans le boîtier à l'envers) dans le fichier Config JSON
- Permutation des numéros de device par défaut des sondes de température 0, 1 et 2
- Ajout des print de température intérieure
- Ajout de 2 commandes HTTP de permutation des numéros de device des sondes de température (0 ↔ 1 et 0 ↔ 2)
- Correction de la gestion des offsets de température par page Web

---

## [2.3.0]

Compatible AsyncTCP v1.0.0 · ESP32 v1.0.0

- Introduction de variantes du nombre de pages OLED (2 ou 4) : `#define DEBUG_OLED`
- Inhibition possible des transmissions à Domoticz : `#define DOMOTICZ_TX`
- Mesure de la température interne sur un 3ᵉ capteur 1-Wire Dallas
- Température affichée avec 1 décimale

---

## [2.2.0]

Compatible AsyncTCP v1.0.0 · ESP32 v1.0.0

- Introduction de variantes en fonction de la taille du display OLED : `#define OLED_096`

---

## [2.1.1]

Compatible AsyncTCP v1.0.0 · ESP32 v1.0.0

- Finetuning

---

## [2.1.0]

- Nouvelle page Web à onglets avec la dernière version de Bootstrap 4.1.3

---

## [2.0.0]

- Version 1.0.0 + Multi-Core (FreeRTOS — task automate sur Core 0)

---

## [1.0.0]

- Version complète et testée — Mono Core

---

## [0.6.0]

- Passage au serveur Web asynchrone
- Amélioration de la gestion des mesures de température aberrantes

---

## [0.3.0]

- Paramétrage inversion écran OLED (`Configuration.flipOLED`)
- Lecture des paramètres réseaux Wi-Fi depuis un fichier SPIFFS `config.json` (SSID / Password)

---

## [0.2.0]

- Première version complète

---

## [0.1.0]

- Création
