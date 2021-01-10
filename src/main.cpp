#include <Arduino.h>
#include <WiFi.h>
#include "../lib/pubsubclient/PubSubClient.h"
#include "../lib/dht/DHT.h"

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

/**
 * MQTT configuration
 */
const char *mqtt_server = "x.x.x.x";
WiFiClient wifiClient;
PubSubClient pubSubClient(wifiClient);


void mqtt_callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
}

void mqtt_reconnect() {
    // Loop until we're reconnected
    while (!pubSubClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (pubSubClient.connect("arduinoClient")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            pubSubClient.publish("outTopic", "hello world");
            // ... and resubscribe
            pubSubClient.subscribe("inTopic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubSubClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


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

void setup_mqtt_client() {
    Serial.println("[Setup MQTT pubSubClient]");
    pubSubClient.setServer(mqtt_server, 1883);
    pubSubClient.setCallback(mqtt_callback);
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    setup_dht_sensor();
    setup_mqtt_client();
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

    pubSubClient.publish("outTopic", ("Temperature: " + String(temperatureCelsius)).begin());

    float heatIndexCelsius = dht.computeHeatIndex(temperatureCelsius, humidity, false);
    float heatIndexFahrenheit = dht.computeHeatIndex(temperatureFahrenheit, humidity);
    Serial.printf("Heat index: %f °C %f °F\n", heatIndexCelsius, heatIndexFahrenheit);
}


void loop() {
    delay(2000);
    if (!pubSubClient.connected()) {
        mqtt_reconnect();
    }
    pubSubClient.loop();
    read_dht_sensor_values();
}
