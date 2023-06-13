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
      
      float myvalue=analogRead(A0) ;
      myvalue= myvalue*0.48828125;
      Serial.println(myvalue);

      for (counter = 1; counter <= 10; counter++) {
        xbeeSerial.println(myvalue);  // Send the temp value to the receiver
        Serial.print("Temp sent: ");
        Serial.println(myvalue);
        delay(3500);  // Delay between transmissions
      }
      xbeeSerial.println('e');  // End of message character
    }
  }
}