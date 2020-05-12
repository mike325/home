// Arduino source code
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// All private information like WIFI/MQTT access
// - WiFi SSID definition
//      const std::map<String, String> SSIDS
// - MQTT definition
//      typedef struct MQTT {
//          const char* host;
//          const char* user;
//          const char* password;
//          const char* fingerprint;
//          const unsigned port;
//      };
//      static const struct MQTT mqtt
#include <access.hpp>

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

const unsigned int BAUD = 115200;
const unsigned short STATUS_LED = 2;
const size_t CAPACITY = JSON_ARRAY_SIZE(15);

const String client_id = "nodemcu";
const String last_will = "clients/" + client_id;
const String subscribe_topics[] = {
    "test/nodemuc"
};

void loop();
void setup();
void connectToWifi();
void connectToMQTT();
void verifyFingerprint();

void connect();
bool connected();
const String getNetwork();

void sendMessage(const String &topic, const String &message);
void sendMessage(const char* topic, const char* message);
void sendMessage(const char* topic, const StaticJsonDocument<CAPACITY> &message);
void callback(const char* topic, const byte* payload, const unsigned int length);

bool connected() {
    return (mqttClient.connected() && espClient.connected());
}

void connect() {
    if (!connected()) {
        connectToWifi();
        connectToMQTT();
    }
}

const String getNetwork() {

#ifdef DEBUG
    Serial.println("Scanning networks");
#endif
    // WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    const byte networks = WiFi.scanNetworks();
    if (networks > 0) {
        for (unsigned i = 0; i < networks; ++i) {
            // Serial.println("------------------------------------------");
            // Serial.print("Name: ");
            // Serial.print(WiFi.SSID(i));
            // Serial.print("; RSSI: ");
            // Serial.println(WiFi.RSSI(i));
            // delay(10);
            if (access::SSIDS.find(WiFi.SSID(i)) != access::SSIDS.end()) {
#ifdef DEBUG
                Serial.println("Found network " + WiFi.SSID(i));
#endif
                return WiFi.SSID(i);
            }
        }
    }

   return "";
}

void connectToWifi() {
    while(WiFi.status() != WL_CONNECTED) {
        const String network = getNetwork();
        if (! network.isEmpty()) {
#ifdef DEBUG
            Serial.print("Connecting to ");
            Serial.print(network);
#endif

            WiFi.scanNetworks();
            WiFi.begin(network, access::SSIDS.at(network));

            while (WiFi.status() != WL_CONNECTED) {
                digitalWrite(STATUS_LED, LOW);
                delay(100);
#ifdef DEBUG
                Serial.print('.');
#endif
                digitalWrite(STATUS_LED, HIGH);
                delay(100);
            }
#ifdef DEBUG
            Serial.println("\n\rConnected To WiFi");
            Serial.print("Local IP: ");
            Serial.println(WiFi.localIP());
#endif
        }
    }
}

void connectToMQTT() {
    if (espClient.connected()) {
        return;
    }

    espClient.setFingerprint(access::mqtt.fingerprint);
    mqttClient.setServer(access::mqtt.host, access::mqtt.port);
    mqttClient.setCallback(callback);

    while (!mqttClient.connected()) {
#ifdef DEBUG
        Serial.println("Attempting MQTT connection...");
#endif
        verifyFingerprint();
        if (mqttClient.connect(client_id.c_str(), access::mqtt.user, access::mqtt.password, last_will.c_str(), 1, true, "0")) {
#ifdef DEBUG
            Serial.println("Connected To MQTT server");
            Serial.println("Sending alive message");
#endif
            mqttClient.publish(last_will.c_str(), "1");
            for (auto i : subscribe_topics) {
#ifdef DEBUG
                Serial.println("Subscribing to " + i);
#endif
                mqttClient.subscribe(i.c_str());
            }
        }
        else {
#ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
#endif
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void verifyFingerprint() {
    if (mqttClient.connected() || espClient.connected()){
        return;
    }

#ifdef DEBUG
    Serial.print("Checking TLS @ ");
    Serial.print(access::mqtt.host);
    Serial.println(" ...");
#endif

    if (!espClient.connect(access::mqtt.host, access::mqtt.port)) {
#ifdef DEBUG
        Serial.println("Connection failed. Rebooting.");
#endif
        Serial.flush();
        delay(1000);
        ESP.restart();
    }

    if (espClient.verify(access::mqtt.fingerprint, access::mqtt.host)) {
#ifdef DEBUG
        Serial.print("Connection secure -> ");
#endif
    }
    else {
#ifdef DEBUG
        Serial.println("Connection insecure! Rebooting.");
#endif
        Serial.flush();
        delay(1000);
        ESP.restart();
    }

    espClient.stop();

    delay(100);
}

void callback(const char* topic, const byte* payload, const unsigned int length) {
#ifdef DEBUG
    Serial.print("Message arrived from: ");
    Serial.println(topic);
#endif

    char message[length + 1];
    for (unsigned int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';

    // StaticJsonDocument<CAPACITY> doc;
    // deserializeJson(doc, payload, length);

// #ifdef DEBUG//
    // Serial.println(message);
// #endif

}

void sendMessage(const String &topic, const String &message) {
    if (topic.isEmpty() || message.isEmpty()) {
#ifdef DEBUG
        Serial.println("-------- Error --------");
        Serial.println("Missing Topic or Message to send");
#endif
        return;
    }

#ifdef DEBUG
    Serial.println("Sending Message");
#endif
    mqttClient.publish(topic.c_str(), message.c_str());

}

void sendMessage(const char* topic, const char* message) {
    if (topic == nullptr || message == nullptr || strlen(topic) == 0 || strlen(message) == 0) {
#ifdef DEBUG
        Serial.println("-------- Error --------");
        Serial.println("Missing Topic or Message to send");
#endif
        return;
    }

#ifdef DEBUG
    Serial.println("Sending Message");
#endif
    mqttClient.publish(topic, message);

}

void sendMessage(const char* topic, const StaticJsonDocument<CAPACITY> &message) {
    if (topic == nullptr || strlen(topic) == 0 || message.isNull()) {
#ifdef DEBUG
        Serial.println("-------- Error --------");
        Serial.println("Missing Topic or Message to send");
#endif
        return;
    }

#ifdef DEBUG
    Serial.println("Sending Message");
#endif

    char buffer[CAPACITY * 2];
    const size_t n = serializeJson(message, buffer);
    mqttClient.publish(topic, buffer, n);

}

void setup() {
    Serial.begin(BAUD);
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, HIGH);

#ifdef DEBUG
    Serial.println("Initilizing Connection");
#endif
    connect();
    digitalWrite(STATUS_LED, LOW);
}

void loop () {

    if (!connected()) {
        digitalWrite(STATUS_LED, HIGH);
#ifdef DEBUG
        Serial.println("Connection Failed, Trying to reconnect");
#endif
        connect();
        digitalWrite(STATUS_LED, LOW);
    }

    mqttClient.loop();

    // char buf[12];
    // itoa(1, buf, 11);
    // Serial.println("Sending 1 to clients/NodeMCU");
    // mqttClient.publish("clients/NodeMCU", buf);
    // counter++;

    delay(1000);
}
