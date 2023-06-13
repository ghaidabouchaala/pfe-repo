//Sender code
#include <SoftwareSerial.h>

SoftwareSerial xbeeSerial(2, 3);  // RX, TX pins for XBee module

int counter = 1;  // Counter variable

void setup() {
  Serial.begin(9600);      // Serial monitor
  xbeeSerial.begin(9600);  // XBee module
}

void loop() {

  if (xbeeSerial.available()) {
    char message = xbeeSerial.read();
    if (message == 's') {
      for (counter = 1; counter <= 10; counter++) {
        xbeeSerial.println(counter);  // Send the counter to the receiver
        Serial.print("Data sent: ");
        Serial.println(counter);
        delay(2000);  // Delay between transmissions
      }
      xbeeSerial.println('e');  // End of message character
    }
  }
}