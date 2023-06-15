#include <SoftwareSerial.h>
#include <XBee.h>
#include <dht11.h>

#define DHT11PIN 7

dht11 DHT11;

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_3_ID = 3;

unsigned long lastTransmissionTime = 0;
unsigned long transmissionInterval = 8000;  

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender 3 node initialized");
}

void loop() {
   // Read data from DHT11 sensor

  // Check if enough time has passed since the last transmission
  if (millis() - lastTransmissionTime >= transmissionInterval) {
    // Create message    
    float humidity_val = readHumidity();

    // Create message packet
    String message = createPacket(SENDER_3_ID, "dht11", String(humidity_val));

    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(1000);  // Small delay to avoid excessive looping
}

float readHumidity() {
  float chk = DHT11.read(DHT11PIN);
  return DHT11.humidity;
}

String createPacket(uint8_t senderID, const String& sensorType, const String& sensorValue) {
  // Create a packet in the format: [SenderID:SensorType:SensorValue]
  String packet = "[" + String(senderID) + ":" + sensorType + ":" + sensorValue + "]";
  return packet;
}
