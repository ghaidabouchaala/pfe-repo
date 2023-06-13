#include <SoftwareSerial.h>

SoftwareSerial xbeeSerial(2, 3);  // RX, TX pins for XBee module

void setup() {
  Serial.begin(9600);      // Serial monitor
  xbeeSerial.begin(9600);  // XBee module
}

void loop() {

  if (Serial.available()) {
    char message = Serial.read();
    if (message == 's') {
      Serial.println("Sending a signal to the sender to start sending counter...");
      xbeeSerial.println(message);  // Send the signal to the sender

      delay(100);  // Small delay to allow the sender to start sending
      String receivedData;

      do {
        if (xbeeSerial.available()) {
          receivedData = xbeeSerial.readStringUntil('\n');
          if (receivedData != "e") {
            Serial.print("Temp received: ");
            Serial.println(receivedData);
          }
        }
      } while (receivedData != "e");

      delay(1000);  // Delay before processing the next received message
    }
  }
}
