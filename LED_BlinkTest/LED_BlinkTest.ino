/*
 * Name: LED_BlinkTest
 * Purpose: To demonstrate on-board LED blinking and ensure the board is good
 * Website: https://www.yeurdreamin.eu  
 * @author Ankit Taneja
 * @version 1.0 04/06/19 
 */

// The inbuilt LED is on Pin 2
#define LED_BUILTIN 2

void setup() {
  // Set the Onboard LED to outout mode
  pinMode(LED_BUILTIN, OUTPUT);
}
 
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
}
