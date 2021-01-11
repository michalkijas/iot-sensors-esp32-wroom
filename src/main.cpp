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


// TODO check F() function: log("[Setup]"); vs log(F("[Setup]"));
void log(String value) {
    Serial.println(value);
}

void log(String value, String endValue) {
    Serial.print(value);
    log(endValue);
}

void log(String value1, String value2, String endValue) {
    Serial.print(value1);
    log(value2, endValue);
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
    log("Message arrived [", topic, "] ");
    String buffer = "";
    for (int i = 0; i < length; i++) {
        buffer += (char) payload[i];
    }
    log("> payload: ", buffer);
}

void mqtt_reconnect() {
    while (!pubSubClient.connected()) {
        log("Attempting MQTT connection");
        if (pubSubClient.connect("arduinoClient")) {
            log("MQTT connected");
            pubSubClient.subscribe("inTopic");
        } else {
            log("MQTT failed, rc=", String(pubSubClient.state()), " try again in 5 seconds");
            delay(5000);
        }
    }
}


void setup_wifi() {
    log("[Setup Wifi]");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        log("> connecting to: ", ssid);
    }
    log("WiFi connected");
    log("IP address: ", WiFi.localIP().toString());
}

void setup_dht_sensor() {
    log("[Setup DHT sensor]");
    dht.begin();
}

void setup_mqtt_client() {
    log("[Setup MQTT pubSubClient]");
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
//    dht.computeHeatIndex(temperatureCelsius, humidity, false);
//    dht.computeHeatIndex(temperatureFahrenheit, humidity);
    if (isnan(humidity) || isnan(temperatureCelsius) || isnan(temperatureFahrenheit)) {
        log("Failed to read from DHT sensor!");
        return;
    }
    log("Humidity: ", String(humidity), "%");
    log("Temperature: ", String(temperatureCelsius), "°C");
    pubSubClient.publish("outTopic", ("Temperature: " + String(temperatureCelsius)).begin());
}


void loop() {
    delay(2000);
    if (!pubSubClient.connected()) {
        mqtt_reconnect();
    }
    pubSubClient.loop();
    read_dht_sensor_values();
}
