/*
 * Name: MakePostToSalesforce
 * Purpose: To demonstrate sending a POST from ESP32 to Salesforce at YeaurDreamin' 19 IoT Workshop
 * Website: https://www.yeurdreamin.eu  
 * @author Ankit Taneja
 * @version 1.0 04/06/19 
 */

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"

/* Provide Wifi details */
//const char* ssid = "";
//const char* password =  "";
const char* ssid = "iPhone";
const char* password =  "Kaktus@2019";
const char* clientId = "3MVG9fTLmJ60pJ5IkfS_Bcx4Q_80FIko5o7Vw1F6q30smb2ttAbTDE3P4WDcnSk_YYovFOhZay4ea3Y0491nK";
char *access_token;  
char *instance_url; 

void setup() {
 
  Serial.begin(115200);
  //Delay needed before calling the WiFi.begin
  delay(4000);   
  WiFi.begin(ssid, password); 
  //Check for the connection
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){   
  
  HTTPClient http;   
  // First Call to get device_code 
  http.begin("https://login.salesforce.com/services/oauth2/token");  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     
  int httpResponseCode = 0;
  char *device_code;
  do {
    char firstPostString[200];
//    strcpy(firstPostString, "response_type=device_code&client_id=");
//    strncat(firstPostString, clientId, strlen(clientId));
//    strcat(firstPostString, "&scope=api");
//    httpResponseCode = http.POST(firstPostString);   //Send the actual POST request
    httpResponseCode = http.POST("response_type=device_code&client_id=3MVG9fTLmJ60pJ5IkfS_Bcx4Q_80FIko5o7Vw1F6q30smb2ttAbTDE3P4WDcnSk_YYovFOhZay4ea3Y0491nK&scope=api");
    if(httpResponseCode > 0){
     
      /* Size of the incoming JSON object-
       * Check https://arduinojson.org/v6/doc/deserialization/ 
       * for more information
       */
      const size_t capacity = JSON_OBJECT_SIZE(4) + 300;
      DynamicJsonDocument doc(capacity);
      String json = http.getString();                         
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, json);
      
      // Check if parsing succeeded
      if (error) {
        // If not, print error
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      } else {
        // Get the device code
//        Serial.print("Device code size:");
//        Serial.println(strlen(doc["device_code"]));
          device_code = strdup(doc["device_code"]);
        
          /* Print the user code
           * to be entered at https://login.salesforce.com/setup/connect
          */
          const char* user_code = doc["user_code"];
          Serial.print("User code:");
          Serial.println(user_code);
        }
    } else {
      Serial.print("Error on getting device code. Response:");
      Serial.println(httpResponseCode);
    }
      http.end();  //Free resources
    } while (httpResponseCode != 200); // Keep asking for User/Device code until successful
    httpResponseCode = 0; 
    delay(2000);
    do {
      // deloy required so that you can enter the user code at the provided link 
      delay(15000); 
      // Ensure device code is there
      if(strlen(device_code) > 100) {
        //Second Call to get Access token
        http.begin("https://login.salesforce.com/services/oauth2/token");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        char secondPostString[300];
        strcpy(secondPostString, "grant_type=device&client_id=3MVG9fTLmJ60pJ5IkfS_Bcx4Q_80FIko5o7Vw1F6q30smb2ttAbTDE3P4WDcnSk_YYovFOhZay4ea3Y0491nK&code=");
        // Add the provided device code to the string
        strncat(secondPostString, device_code, strlen(device_code)); 
        Serial.print("PostString:");
        Serial.println(secondPostString);
        httpResponseCode = http.POST(secondPostString);
        String json;
        if(httpResponseCode > 0) {
        const size_t capacity = JSON_OBJECT_SIZE(7) + 410;
        DynamicJsonDocument doc(capacity);  
        json = http.getString();
        deserializeJson(doc, json);
        access_token = strdup(doc["access_token"]);
        instance_url = strdup(doc["instance_url"]);
        Serial.print("Atoken:");
        Serial.println(access_token);
        
      } else {
        Serial.println(json);
      }
        // IF access token received, break.
        if(strlen(access_token) > 100) { break; }
      }
     } while( httpResponseCode != 200);
     http.end();
   } else {
      Serial.println("Error in WiFi connection");   
   }
}
 
void loop() {
  // Setup the touch Pin to D4 and start listening
  int touchValue = touchRead(T0);
  char instanceUrl[200];
  const char *second = "/services/data/v45.0/sobjects/Flying_Fridge_Event__e"; 
  strcpy(instanceUrl, instance_url);
  strcat(instanceUrl, second);
  
  char token[200];
  strcpy(token, "Bearer ");
  strcat(token, access_token);
  
  Serial.print("Bearer:");
  Serial.println(token);
  Serial.print("TouchValue:");
  Serial.println(touchRead(T0));
  if(touchValue < 30) {
    
    // Create a JSON for platform event
    const size_t capacity = JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);
    
    doc["serial_no__c"] = "1001";
    doc["door_open__c"] = true;
    doc["temperature__c"] = touchValue;
    
    String output;
    serializeJson(doc, output);
    HTTPClient http; 
    Serial.println(instanceUrl);
    http.begin(instanceUrl);
    
    Serial.println(token);
    http.addHeader("Authorization", token);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.POST(output);
    Serial.println(responseCode);
    String response = http.getString();
    Serial.println(response);
    http.end();
  }

  delay(5000);  //Keep looking every 5 seconds for change in touch value
}
