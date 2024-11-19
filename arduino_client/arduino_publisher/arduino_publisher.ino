#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

#define ssid       "classroom-B"
#define password   "Nptu123456789"

#define brokerHost "your_broker_host"
#define brokerPort 1883
#define clientID   "Arduino_MQTT_Publisher"

#define ledPin     10
#define buttonPin  8

WiFiClient wifi;
MqttClient mqttClient(wifi);

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  connectToWiFi();
  connectToBroker();
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected!\nReconnect to WiFi");
    connectToWiFi();
    return;
  }
  if(!mqttClient.connected()) {
    connectToBroker();
  }
  led();
}

void connectToWiFi() {
  Serial.print("Try to connect to WiFi :");
  Serial.println(ssid);
  while(WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connect WiFi scuuessfully !");
}

void connectToBroker() {
  Serial.print("Try to connect to MQTTBroker :");
  Serial.println(brokerHost);
  mqttClient.setId(clientID);
  while(!mqttClient.connect(brokerHost, brokerPort)) {
    Serial.print("MQTT connection failed ! Error code = ");
    Serial.println(mqttClient.connectError());
  delay(1000);
  return;
  }
  Serial.println("Connect to broker scuuessfully !");
}

void led() {
  digitalWrite(ledPin, HIGH);
  mqttClient.beginMessage("led");
  mqttClient.print("HIGH");
  mqttClient.endMessage();
  delay(10000);
  digitalWrite(ledPin, LOW);
  mqttClient.beginMessage("led");
  mqttClient.print("LOW");
  mqttClient.endMessage();
  delay(1000);
}
