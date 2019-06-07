
/**
  Name: mqttPublish
  Purpose: To demonstrate Subscribe/Publish mqtt properties at YeaurDreamin 19
  Website: https://www.yeurdreamin.eu  
  @author Ankit Taneja
  @version 1.0 04/06/19 
*/
#include <WiFi.h>
#include <PubSubClient.h>
#define LED_BUILTIN 2
 
const char* ssid = "iPhone";
const char* password =  "Kaktus@2019";
const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 12004;
const char* mqttUser = "ntqxchaq";
const char* mqttPassword = "qcs-8ry6gnWw";

 
WiFiClient espClient;
PubSubClient client(espClient);
 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);
  
}
 
void setup() {
  // Set the Onboard LED to outout mode
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESPClient", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
  client.subscribe("hallo/amsterdam");
}
 
void loop() {
  client.loop();
  //Assign the touch pin
  int touch = touchRead(T0);
  if(touch < 30) {
    Serial.println("Publishing to Host");
    char str[3];
    itoa(touch, str, 10);
    client.publish("bonjour/host",str); 
  }
}
