#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_4_ID = 4;

unsigned long lastTransmissionTime = 0;
unsigned long transmissionInterval = 7000;  // Time interval between sending data (5 seconds)

const int trigPin = 9;  
const int echoPin = 10; 
float duration, distance; 

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);  
  Serial.println("Sender 4 node initialized");
}

void loop() {
  
  // Check if enough time has passed since the last transmission
  if (millis() - lastTransmissionTime >= transmissionInterval) {
    // Create message

    float distance = readDistance();

    // Create message packet
    String message = createPacket(SENDER_4_ID, "HC-SR04", String(distance));
    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(100);  // Small delay to avoid excessive looping
}


float readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  return distance;
}

String createPacket(uint8_t senderID, const String& sensorType, const String& sensorValue) {
  // Create a packet in the format: [SenderID:SensorType:SensorValue]
  String packet = "[" + String(senderID) + ":" + sensorType + ":" + sensorValue + "]";
  return packet;
}
