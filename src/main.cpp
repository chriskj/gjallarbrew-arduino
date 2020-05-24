#include <Arduino.h>
#include <wifi.h>
#include <MQTT.h>
#include <WiFiClientSecure.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>

#define ONE_WIRE_BUS 13 // GPIO port number


WiFiClientSecure net;
MQTTClient mqtt;

float Celcius1=0;
float Celcius2=0;
float Celcius3=0;

unsigned long lastMillis = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void mqttconnect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nMQTT connecting...");
  while (!mqtt.connect(mqtt_clientid, mqtt_username, mqtt_password)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nMQTT connected!");
  mqtt.subscribe("ckj/#");
  // mqtt.unsubscribe("ckj/#");

}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}


void setup() {
  Serial.begin(115200);

  // Connect to wifi
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("WiFi Connected!");

  // Initialize the temperature sensors
  sensors.begin();

  // Connect to MQTT
  net.setPreSharedKey(mqtt_username, mqtt_password);
  
  mqtt.begin(mqtt_server, mqtt_port, net);
  mqtt.onMessage(messageReceived);

  mqttconnect();
}


void loop() {
  sensors.requestTemperatures();
  
  Celcius1 = sensors.getTempC(sensor1);
  Celcius2 = sensors.getTempC(sensor2);
  Celcius3 = sensors.getTempC(sensor3);

  mqtt.loop();
  // publish a message roughly every second.
  if (millis() - lastMillis > 500) {
    lastMillis = millis();
    mqtt.publish("ckj/gjallarbrew/sensor1", String(Celcius1));
    mqtt.publish("ckj/gjallarbrew/sensor2", String(Celcius2));
    mqtt.publish("ckj/gjallarbrew/sensor3", String(Celcius3));
    Serial.println("publishing message: ");

  }
  delay(2000);
  
  // // Search for addresses

  // byte i;
  // byte addr[8];

  // if (!oneWire.search(addr)) {
  //   Serial.println(" No more addresses.");
  //   Serial.println();
  //   oneWire.reset_search();
  //   delay(4000);
  //   return;
  // }
  // Serial.print(" ROM =");
  // for (i = 0; i < 8; i++) {
  //   Serial.write(' ');
  //   Serial.print(addr[i], HEX);
  // }

}
