#include <Arduino.h>
#include <WiFi.h>
#include "DHT.h"

/**
 * WiFi configuration
 */
const char *ssid = "***";
const char *password = "***";

/**
 * DHT sensor configuration
 */
#define DHT_PIN 17
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

void setup_wifi() {
    Serial.println("[Setup Wifi]");
    Serial.print("Connecting to: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup_dht_sensor() {
    Serial.println("[Setup DHT sensor]");
    dht.begin();
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    setup_dht_sensor();
}


void read_dht_sensor_values() {
    float humidity = dht.readHumidity();
    float temperatureCelsius = dht.readTemperature();
    float temperatureFahrenheit = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperatureCelsius) || isnan(temperatureFahrenheit)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    Serial.printf("Humidity: %f %%, Temperature: %f °C %f °F\n", humidity, temperatureCelsius, temperatureFahrenheit);

    float heatIndexCelsius = dht.computeHeatIndex(temperatureCelsius, humidity, false);
    float heatIndexFahrenheit = dht.computeHeatIndex(temperatureFahrenheit, humidity);
    Serial.printf("Heat index: %f °C %f °F\n", heatIndexCelsius, heatIndexFahrenheit);
}


void loop() {
    delay(2000);
    read_dht_sensor_values();
}
