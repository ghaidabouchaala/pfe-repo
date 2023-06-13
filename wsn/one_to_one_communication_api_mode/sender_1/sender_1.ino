#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

void setup() {
  Serial.begin(9600);         // Initialize the serial monitor
  xbeeSerial.begin(9600);     // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender node initialized");
}

void loop() {
  String data = "Hello from sender 1!";
  XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);  // Replace with the receiver's XBee address
  ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)data.c_str(), data.length());

  Serial.print("Sending data to receiver address: ");
  Serial.print(receiverAddr.getMsb(), HEX);
  Serial.print(" ");
  Serial.println(receiverAddr.getLsb(), HEX);
  
  Serial.println("Sending data...");
  xbee.send(tx);
  Serial.println("Data sent.");
  
  delay(5000);  // Time interval between sending data (5 seconds)
}
