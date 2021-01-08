#include <Arduino.h>
#include "DHT.h"

#define DHT_PIN 17
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);
    Serial.println("Setup");
    dht.begin();
}

void loop() {
    delay(2000);

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
