#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

#define ssid       ""
#define password   ""

#define brokerHost ""
#define brokerPort 
#define clientID   ""

#define ledPin     10
#define buttonPin  8
#define potPin     A0

int potValue        = 0;
int buttonState     = 0;
int lastButtonState = HIGH;
unsigned long previousMillis = 0;
bool ledState = false;


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
  sensor();
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

void sensor() {
  potValue = analogRead(potPin);
  potValue = map(potValue,0,1023,0,5000);
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH){
    mqttClient.beginMessage("potValue");
    mqttClient.print(potValue);
    mqttClient.endMessage();
    lastButtonState == buttonState;
  }
}

void led() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= potValue) {
    previousMillis = currentMillis;

    ledState = !ledState;
    digitalWrite(ledPin, ledState ? HIGH : LOW);

    mqttClient.beginMessage("led");
    mqttClient.print(ledState ? "HIGH" : "LOW");
    mqttClient.endMessage();
  }
}
