#include <XBee.h>

XBee xbee = XBee();
HardwareSerial& xbeeSerial = Serial1;  // Use Serial1 for XBee communication

// Sender XBee addresses
XBeeAddress64 sender1Addr = XBeeAddress64(0x0013A200, 0x41957E34);
XBeeAddress64 sender2Addr = XBeeAddress64(0x0013A200, 0x41957E54);

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Receiver node initialized");
}

void loop() {
  if (xbeeSerial.available()) {
    Serial.println("XBee serial is available");
    
    XBeeResponse response = xbee.getResponse();
    Serial.print("Received response with API ID: ");
    Serial.println(response.getApiId(), HEX);
    
    if (response.isAvailable()) {
      Serial.println("Response is available");
      ZBRxResponse rx;
      response.getZBRxResponse(rx);

      // Print received XBee address
      Serial.print("Received from XBee address: ");
      XBeeAddress64 senderAddr = rx.getRemoteAddress64();
      Serial.print(senderAddr.getMsb(), HEX);
      Serial.print(" ");
      Serial.println(senderAddr.getLsb(), HEX);

      // Identify the sender based on the received address
      uint8_t senderId = 0;
      if (senderAddr == sender1Addr) {
        senderId = 1;
      } else if (senderAddr == sender2Addr) {
        senderId = 2;
      }

      // Print sender ID
      Serial.print("Sender ID: ");
      Serial.println(senderId);

      // Print received data to serial monitor
      Serial.print("Received data: ");
      for (int i = 0; i < rx.getDataLength(); i++) {
        Serial.write(rx.getData(i));
      }
      Serial.println();
    }
    else {
      Serial.println("Response is not available");
    }
  }

  delay(5000);  // Add a delay of 5 seconds between each check
}
