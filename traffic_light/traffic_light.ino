#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"

const char* ssid = "Room105";
const char* password = "indonesiaraya";
const char* mqttServer = "broker.emqx.io";
int port = 1883;
const char* topic_p = "traffic_light/rizkyproject2023";
const char* topic_s = "traffic_delay/rizkyproject2023";

int red_delay = 100;
int green_delay = 100;

String stMac;
char mac[50];
char clientId[50];

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

const int pinRed = D5;
const int pinYellow = D3;
const int pinGreen = D4;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  wifiConnect();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  stMac = WiFi.macAddress();
  stMac.replace(":", "_");
  Serial.println(stMac);
  
  client.setServer(mqttServer, port);
  client.setCallback(callback);
  pinMode(pinRed, OUTPUT);
  pinMode(pinYellow, OUTPUT);
  pinMode(pinGreen, OUTPUT);
}

void wifiConnect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.print(clientId);
      Serial.println(" connected");
      client.subscribe(topic_s);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String strPayload;

  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    strPayload += (char)payload[i];
  }
  
  char json[500];
  strPayload.toCharArray(json, 500);
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));
  deserializeJson(doc, json);
  red_delay = doc["red_delay"];
  green_delay = doc["green_delay"];

  Serial.print("Red Delay: ");
  Serial.println(red_delay);
  Serial.print("Green Delay: ");
  Serial.println(green_delay);
  Serial.println();   
}

void stop(int timeDelay){
  client.publish(topic_p, "100");
  digitalWrite(pinRed, HIGH);
  digitalWrite(pinYellow, LOW);
  digitalWrite(pinGreen, LOW);
  delay(timeDelay);
}
void slow(int timeDelay){
  client.publish(topic_p, "011");
  digitalWrite(pinRed, LOW);
  digitalWrite(pinYellow, HIGH);
  digitalWrite(pinGreen, HIGH);
  delay(timeDelay);
}
void go(int timeDelay){
  client.publish(topic_p, "001");
  digitalWrite(pinRed, LOW);
  digitalWrite(pinYellow, LOW);
  digitalWrite(pinGreen, HIGH);
  delay(timeDelay);
}

void loop() {
  delay(10);
  if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    go(green_delay);
    slow(1000);
    stop(red_delay);
  }
}
