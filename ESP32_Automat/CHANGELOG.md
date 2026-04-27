# Changelog — ESP32_Automat (Automatisme piscine)

Toutes les modifications notables sont documentées dans ce fichier.

---

## [2.4.0] — 2026-04-27 (en cours)

Compatible AsyncTCP v3.4.10 · ESPAsyncWebServer v3.1.0 · ESP32 v3.3.8 · ESP8266 v3.1.2

### Synchronisation FreeRTOS (dual-core)

- Nouveau mutex global **`stateMutex`** (`SemaphoreHandle_t`) créé dans `setup()` avant le lancement de `AutomatTaskCode`
- `AutomatTaskCode` (Core 0) : `DisplayWaterTemperatureOnLED` + `AutomatRun` protégés par `xSemaphoreTake/Give(stateMutex)` — élimine les data races sur `PoolState`, `Automat_Mode`, `Automat_Cmd`
- `delay(1000)` remplacé par `vTaskDelay(pdMS_TO_TICKS(1000))` — rend la main au scheduler au lieu de bloquer Core 0
- `ParseString.ino` : 4 blocs d'écriture cross-core (`PoolState.AirTemp`, `.WaterTemp`, `Automat_Mode.ModeState`, `Automat_Cmd.CommandState`) protégés par mutex
- `Temperatures.ino` (`AcquireTemperatures`) : écritures sur `PoolState` protégées par mutex ; `readTempRaw` (accès 1-Wire lent) exécuté hors mutex
- `Automat.ino` (`MeasurePeriodOfCold`) : lectures de `PoolState` et écriture de `PeriodOfLowAirTemp` protégées par mutex
- `WebServer.ino` : snapshot atomique de l'état partagé sous mutex dans `handleStatusJSON` et `handleTextInfo`

### Wi-Fi

- `isMyWiFi()` et `myWiFiPassword()` : correction off-by-one (`< nbWiFiNetworks - 1` → `< nbWiFiNetworks`) — le dernier réseau configuré n'était jamais testé
- `ConnectToWiFi()` : garde `if (selectedWiFi < 0) return false` avant `WiFi.SSID(selectedWiFi)` — évite un comportement indéfini (crash) si aucun réseau connu n'est dans la portée

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
- Migration **ArduinoJson v5 → v7** (`StaticJsonBuffer` → `JsonDocument`, `parseObject` → `deserializeJson`)

### Fiabilisation des mesures de température Dallas (1-Wire)

- Précision réduite de **11 → 10 bits** (`TEMPERATURE_PRECISION`) pour accélérer la conversion
- Délai de mesure augmenté de **+100 ms** (`delaySamplingTemp`) pour laisser le temps aux capteurs de convertir
- Nouvelle fonction **`readTempRaw(deviceIndex)`** : lecture avec retry automatique si la valeur retournée est -127 °C (erreur CRC) ou 85 °C (valeur de mise sous tension parasite)
- Borne physique absolue dans `readTempRaw()` : toute valeur hors de `[-10 ; 60] °C` retourne `DEVICE_DISCONNECTED_C` — protège contre les valeurs corrompues mais numériquement plausibles qui passeraient le filtre delta
- Nouvelle fonction **`isSensorInitOK(deviceID)`** : vérifie si un capteur a été initialisé sans erreur avant de le lire
- `setResolution()` conditionné capteur par capteur via `isSensorInitOK()` — évite d'envoyer une trame invalide sur le bus 1-Wire
- `SampleTemperatures()` : déclenchement rendu indépendant du flag global `ErrorTempSensorInit` — les capteurs fonctionnels sont toujours lus même si un autre a échoué à l'init
- `AcquireTemperatures()` et `InitTemperatureSensors()` : lectures conditionnées capteur par capteur

### Robustesse du bus I2C

- Bus I2C initialisé avec **timeout de 3 ms** (`Wire.setTimeOut(3)`) dans `initializeOLED()`
- Nouvelle fonction **`recoverI2C()`** : récupération du bus I2C bloqué par 9 impulsions d'horloge SCL, réinit `Wire` et display OLED — appelée automatiquement dans `loop()` si `ui.update()` retourne < -500
- `recoverI2C()` : ajout de `ui.init()` avant `display.init()` pour réinitialiser l'état interne de `OLEDDisplayUi` (position de frame, animation) — sans ça, l'écran affiche du bruit graphique après récupération
- `recoverI2C()` : ajout de `display.setFont(ArialMT_Plain_10)` pour restaurer la police par défaut effacée par le reset hardware

### Affichage OLED

- Fréquence OLED réduite de **25 fps → 20 fps**
- Nouvelle frame **`drawWaterTemperatures()`** : affiche la température de l'eau en grand avec la police `Dialog_bold_32` (nouveau fichier `font.h`)
- En mode non-debug : `drawDeviceInfoTemperatures` remplacée par `drawWaterTemperatures`
- Remplacement des concaténations `String +` par `snprintf()` + buffer pour éviter les allocations dynamiques

### Logique relais

- Ajout des constantes `RELAY_OPEN = HIGH` et `RELAY_CLOSED = LOW` — les relais sont à logique inversée (bobine au repos = HIGH, bobine excitée = LOW)
- `IO.ino` : `SwitchRelayAutoManu()` et `SwitchRelayOpenCloseCover()` utilisent les nouvelles constantes — plus de `HIGH`/`LOW` nus sur les sorties relais
- `SendData.ino` : comparaisons `== HIGH` remplacées par `== RELAY_OPEN`
- Valeurs initiales de `Relay1` / `Relay2` mises à jour en `RELAY_OPEN`

### Refactoring état partagé

- Nouveau struct **`SharedState_T`** regroupant `mutex`, `pool` (`PoolState_T`), `mode` (`Automat_Mode_T`) et `cmd` (`Automat_Cmd_T`) — le mutex est désormais co-localisé avec les données qu'il protège
- Instance globale unique `SharedState` remplace les quatre déclarations séparées (`stateMutex`, `PoolState`, `Automat_Mode`, `Automat_Cmd`)
- Quatre **aliases C++ par référence** (`PoolState_T& PoolState = SharedState.pool;` etc.) assurent une rétrocompatibilité totale — aucun autre fichier modifié

### Gestion mémoire et initialisation

- `Configuration_T.WiFiNetworks` : initialisé à `nullptr` dans la définition du struct — protège contre les accès avant que `ReadConfig()` ait été appelé
- `JSON.ino` (`ReadConfig`) : `delete[] + nullptr` avant chaque `new WiFiNetwok_T[]` — supprime la fuite mémoire en cas de rechargement de config (ex. OTA)
- `Preferences` : namespace NVS conservé ouvert intentionnellement toute la session — `get/put` sont appelés en continu depuis des timers et des handlers HTTP ; le handle est libéré proprement avant chaque restart
- `millis()` overflow : confirmé non-problème — `prevMillis` et `currentMillis` sont `unsigned long`, la soustraction unsigned gère le rollover à 49 jours

### Qualité de code

- Typo corrigée : `WiFiNetwok_T` → `WiFiNetwork_T` dans `ESP32_Automat.ino` et `JSON.ino`
- Mot de passe AP (`automat_pwd`) retiré du code source — valeur par défaut `"Levsmsa2"` supprimée du struct et du fallback `ReadConfig()` ; le mot de passe vit uniquement dans `config.json` sur le SPIFFS (`"access point password"`). Si la clé est absente, l'AP démarre ouvert avec un avertissement série
- `ESP32_Automat.ino` : prototypes explicites ajoutés pour `AirTempDeviceID()`, `WaterTempDeviceID()`, `InternalTempDeviceID()`, leurs setters et `StringPreferences()` — fonctions définies dans `myPreferences.ino` qui compile après `Temperatures.ino` et `WebServer.ino`
- Ajout de la commande `/info` dans la page de texte d'information du serveur Web
- Ajout du bitmap **`alert_bits`** (16×16) dans `images.h`
- `InitTemperatureSensors()` : `delay(2000)` déplacé après `DisplayAlert()` — affichage d'abord, pause ensuite
- Période d'exécution de la task automate : **500 ms → 1 s**

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
