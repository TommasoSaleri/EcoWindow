#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Stepper.h>

// IMPOSTAZIONI WIFI
const char* ssid = "IL_TUO_WIFI";
const char* password = "LA_TUA_PASSWORD";

// IMPOSTAZIONI SENSORI E PIN
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define GAS_PIN 34
#define LED_PIN 5 

const float SOGLIA_CALDO = 25.0;
const float SOGLIA_FREDDO = 17.0;
const int SOGLIA_GAS = 3100;

// IMPOSTAZIONI MOTORI
const int stepsPerRevolution = 2048;
Stepper motorTapparella(stepsPerRevolution, 13, 14, 12, 27); 
Stepper motorFinestra(stepsPerRevolution, 26, 33, 25, 32);

bool isWindowOpen = false;
bool isBlindUp = true;

// IMPOSTAZIONI API OPENMETEO
unsigned long lastApiCallTime = 0;
const unsigned long apiInterval = 15 * 60 * 1000;

String latitude = "45.53"; // coordinate di Brescia, modificare i parametri se necessario
String longitude = "10.21";
String openMeteoUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&current=temperature_2m,is_day,weather_code";

float tempEsterna = 0.0;
bool isDay = true;
int weatherCode = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  dht.begin();
  motorTapparella.setSpeed(10);
  motorFinestra.setSpeed(10);

  Serial.print("Connessione al WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connesso!");
  
  fetchOpenMeteoData();
  eseguiLogicaDiControllo();
}

void loop() {
  if (millis() - lastApiCallTime >= apiInterval) {
    fetchOpenMeteoData();
    lastApiCallTime = millis();
  }

  int gasValue = analogRead(GAS_PIN);
  float tempInterna = dht.readTemperature();

  Serial.println("--- STATO SISTEMA ---");
  Serial.print("Gas: "); Serial.print(gasValue);
  if (gasValue > SOGLIA_GAS) Serial.println(" !!! ALLARME !!!"); else Serial.println(" OK");
  Serial.print("Temp Int: "); Serial.print(tempInterna); Serial.println(" C");
  Serial.print("Temp Est (API): "); Serial.print(tempEsterna); Serial.println(" C");
  Serial.print("Finestra: "); Serial.println(isWindowOpen ? "APERTA" : "CHIUSA");
  Serial.print("Tapparella: "); Serial.println(isBlindUp ? "SU" : "GIU");
  Serial.println("---------------------");

  if (gasValue > SOGLIA_GAS) {
    AllarmeGas(gasValue); 
  } else {
    digitalWrite(LED_PIN, LOW);
    eseguiLogicaDiControllo(); 
  }
  
  delay(5000);
}

void fetchOpenMeteoData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(openMeteoUrl);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      tempEsterna = doc["current"]["temperature_2m"];
      isDay = (doc["current"]["is_day"] == 1);
      weatherCode = doc["current"]["weather_code"];
    }
    http.end();
  }
}

void eseguiLogicaDiControllo() {
  float tempInterna = dht.readTemperature();
  if (isnan(tempInterna)) return;

  bool isBadWeather = (weatherCode >= 50);
  if (!isDay || (isDay && isBadWeather)) { 
    muoviTapparella(false); 
  } else { 
    muoviTapparella(true);
  }

  bool apriFinestra = false;
  if (tempInterna < SOGLIA_FREDDO && tempEsterna > tempInterna) {
    apriFinestra = true;
  } else if (tempInterna > SOGLIA_CALDO && tempEsterna < tempInterna) {
    apriFinestra = true;
  } 
  muoviFinestra(apriFinestra);
}

void AllarmeGas(int valoreGas) {
  digitalWrite(LED_PIN, HIGH);
  muoviTapparella(true);
  muoviFinestra(true);
}

void muoviTapparella(bool apri) {
  if (apri && !isBlindUp) {
    motorTapparella.step(stepsPerRevolution * 2);
    isBlindUp = true;
  } else if (!apri && isBlindUp) {
    motorTapparella.step(-stepsPerRevolution * 2);
    isBlindUp = false;
  }
}

void muoviFinestra(bool apri) {
  if (apri && !isWindowOpen) {
    motorFinestra.step(stepsPerRevolution); 
    isWindowOpen = true;
  } else if (!apri && isWindowOpen) {
    motorFinestra.step(-stepsPerRevolution);
    isWindowOpen = false;
  }
}
