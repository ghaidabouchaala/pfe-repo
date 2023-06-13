#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_2_ID = 2;

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender 2 node initialized");
}

void loop() {

  // Read data from serial monitor
  String data = "This is sender 2!";

  // Create XBee transmission request
  ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)data.c_str(), data.length());

  Serial.println("Sending data from Sender 2...");
  xbee.send(tx);
  Serial.println("Data sent from Sender 2.");
  
  delay(5000);  // Time interval between sending data (5 seconds)
}
