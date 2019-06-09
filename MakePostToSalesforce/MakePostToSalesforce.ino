/*
 * Name: MakePostToSalesforce
 * Purpose: To demonstrate sending a POST from ESP32 to Salesforce at YeaurDreamin' 19 IoT Workshop
 * Website: https://www.yeurdreamin.eu  
 * @author Ankit Taneja
 * @version 1.0 04/06/19 
 */

#include "WiFi.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"

/* Provide Wifi details */
//const char* ssid = "";
//const char* password =  "";
const char* ssid = "iPhone";
const char* password =  "Kaktus@2019";

/* Provide Auth details */
const char* clientId = "3MVG9MHOv_bskkhRWMoOCuaVXBqhIvP0c3fJlDJU06CZoXq7TnsZuoVJTqseqFPKeHhWbPMGytkl_oBq0srtF";
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
  http.begin("https://test.salesforce.com/services/oauth2/token");  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     
  int httpResponseCode = 0;
  char *device_code;
  do {
    char firstPostString[200];
    strcpy(firstPostString, "response_type=device_code&client_id=");
    strncat(firstPostString, clientId, strlen(clientId));
    strcat(firstPostString, "&scope=api");
//    Serial.print("Str:");
//    Serial.println(firstPostString);
    httpResponseCode = http.POST(firstPostString);   //Send the actual POST request
//    httpResponseCode = http.POST("response_type=device_code&client_id=3MVG9MHOv_bskkhRWMoOCuaVXBqhIvP0c3fJlDJU06CZoXq7TnsZuoVJTqseqFPKeHhWbPMGytkl_oBq0srtF&scope=api");
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
          device_code = strdup(doc["device_code"]);
          /* Print the user code
           * to be entered at https://test.salesforce.com/setup/connect
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
      // 30s deloy so that you can enter the user code at the provided link 
      delay(30000); 
      // Ensure device code is there
      if(strlen(device_code) > 100) {
        //Second Call to get Access token
        http.begin("https://test.salesforce.com/services/oauth2/token");  
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        char secondPostString[300];
        strcpy(secondPostString, "grant_type=device&client_id=");
        strncat(secondPostString, clientId, strlen(clientId));
        strncat(secondPostString, "&code=", 6);
        // Add the provided device code to the string
        strncat(secondPostString, device_code, strlen(device_code)); 

        httpResponseCode = http.POST(secondPostString);
        String json;
        if(httpResponseCode > 0) {
          const size_t capacity = JSON_OBJECT_SIZE(7) + 410;
          DynamicJsonDocument doc(capacity);  
          json = http.getString();
          deserializeJson(doc, json);
          access_token = strdup(doc["access_token"]);
          instance_url = strdup(doc["instance_url"]);
          
        } else {
          Serial.println(json);
        }
        // IF access token received, break.
        if(strlen(access_token) > 100) { break; }
      }
     } while( httpResponseCode != 200);
     http.end(); // Free resources when done
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
    http.begin(instanceUrl);
    
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
