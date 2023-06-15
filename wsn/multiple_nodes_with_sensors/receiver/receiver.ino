#include <XBee.h>

XBee xbee = XBee();
HardwareSerial& xbeeSerial = Serial1;  // Use Serial1 for XBee communication

void setup() {
  Serial.begin(9600);         // Initialize the serial monitor
  xbeeSerial.begin(9600);     // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Receiver node initialized");
}

void loop() {
  if (xbeeSerial.available()) {
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        ZBRxResponse rx;
        xbee.getResponse().getZBRxResponse(rx);
  
        // Print received XBee address
        XBeeAddress64 senderAddr = rx.getRemoteAddress64();
        Serial.print("Received from XBee address: ");
        Serial.print(senderAddr.getMsb(), HEX);
        Serial.print(" ");
        Serial.println(senderAddr.getLsb(), HEX);
  
        // Print received data to serial monitor
        Serial.print("Received data: ");
        for (int i = 0; i < rx.getDataLength(); i++) {
          Serial.write(rx.getData(i));
        }
        Serial.println();
      }
    }
  }

  delay(500);  // Add a short delay between each check
}