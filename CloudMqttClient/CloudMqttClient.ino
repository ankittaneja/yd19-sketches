/*
 * Name: mqttClient
 * Purpose: To demonstrate Mqtt Publish / Subscribe and receiving messsages Salesforce at YeaurDreamin' 19 IoT Workshop
 * Website: https://www.yeurdreamin.eu  
 * @author Ankit Taneja
 * @version 1.0 04/06/19 
 */


#include <WiFi.h>
#include <PubSubClient.h>

// Define the Pin for LED
#define LED_PIN 4
/* Declare constants i.e. Wifi + mqtt server details
 * Check https://yd19.herokuapp.com/ for more details   
 *  
 */
const char* ssid = "iPhone";
const char* password =  "Kaktus@2019";
const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 12004;
const char* mqttUser = "ntqxchaq";
const char* mqttPassword = "qcs-8ry6gnWw";

WiFiClient espClient;
PubSubClient client(espClient);

// function to be called everytime a message is received
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
  // Blink the LED to show message received
  digitalWrite(LED_PIN, HIGH);
  delay(5000);
  digitalWrite(LED_PIN, LOW);
}
 
void setup() {
  // Set the LED to output mode
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Connect to the mqttServer
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("YeurClient", mqttUser, mqttPassword )) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // Subscribe to a topic
  client.subscribe("hallo/amsterdam");
}

void loop() {
  client.loop();
}
