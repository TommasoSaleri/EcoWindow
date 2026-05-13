# EcoWindow 🏠

**EcoWindow** è un sistema di domotica intelligente basato su **ESP32** progettato per automatizzare l'apertura di una finestra e il movimento della sua tapparella. Il sistema incrocia i dati meteorologici in tempo reale (tramite l'API di OpenMeteo) con le letture dei sensori locali per ottimizzare l'efficienza energetica e la sicurezza della casa.

## ✨ Funzionalità
- **Gestione Climatica:** Confronta la temperatura interna ed esterna per decidere se aprire la finestra (raffrescamento/riscaldamento naturale).
- **Automazione Tapparella:** Si chiude automaticamente di notte o in caso di maltempo (pioggia, neve, temporali) basandosi sui dati API.
- **Sicurezza Gas:** In caso di rilevamento fumi o gas sopra la soglia, il sistema attiva un LED di allarme, alza la tapparella e apre la finestra per ventilare l'ambiente.
- **Monitoraggio in tempo reale:** Tutti i dati (Gas, Temperatura, Stato Motori) sono visualizzati sul Serial Monitor.

## 🛠️ Hardware Necessario
- **Microcontrollore:** ESP32 WROOM (30 pin).
- **Sensore Ambiente:** DHT11 (Temperatura e Umidità).
- **Sensore Gas:** Serie MQ (es. MQ-2).
- **Attuatori:** 2x Motori Passo-Passo 28BYJ-48 con driver ULN2003.
- **Segnalazione:** LED (con resistenza da 220Ω).
- **Alimentazione:** Cavo Micro-USB o alimentatore esterno 5V (consigliato per i motori).

## 📌 Schema di Collegamento
| Componente | Pin ESP32 | Note |
| :--- | :--- | :--- |
| **DHT11 (Data)** | GPIO 4 | Alimentare a 3.3V o 5V |
| **Sensore Gas (A0)** | GPIO 34 | Collegare a VIN (5V) |
| **LED Allarme** | GPIO 5 | Usare resistenza in serie |
| **Motore Tapparella** | GPIO 13, 12, 14, 27 | Driver ULN2003 (IN1-IN4) |
| **Motore Finestra** | GPIO 26, 25, 33, 32 | Driver ULN2003 (IN1-IN4) |

## 💻 Configurazione Software

### 1. Preparazione Arduino IDE
Se non hai mai usato l'ESP32 con l'IDE di Arduino:
1. Vai su **File > Impostazioni**.
2. Nel campo "URL aggiuntive per il Gestore schede", inserisci:  
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Vai su **Strumenti > Scheda > Gestore schede...**, cerca **esp32** e installa la versione di Espressif.
4. Seleziona la tua scheda (es. `DOIT ESP32 DEVKIT V1`).

### 2. Librerie richieste
Cerca e installa nel "Gestore Librerie":
- **DHT sensor library** (Adafruit)
- **Adafruit Unified Sensor** (dipendenza del DHT)
- **ArduinoJson** (Benoit Blanchon)
- **Stepper** (integrata nell'IDE)

### 3. Personalizzazione del codice
Prima di caricare il codice, modifica le seguenti variabili:
```cpp
const char* ssid = "IL_TUO_WIFI";
const char* password = "LA_TUA_PASSWORD";
String latitude = "45.53";  // Le tue coordinate
String longitude = "10.21";
