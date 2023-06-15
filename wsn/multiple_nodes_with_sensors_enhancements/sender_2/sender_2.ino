#include <SoftwareSerial.h>
#include <XBee.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_2_ID = 2;

unsigned long lastTransmissionTime = 0;
unsigned long transmissionInterval = 6000;  

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender 2 node initialized");
}

void loop() {
  sensors.requestTemperatures(); // Request temperature data from Dallas temperature sensor

  // Check if enough time has passed since the last transmission
  if (millis() - lastTransmissionTime >= transmissionInterval) {

    float temp_val = readTemperature(); 

    // Create message packet
    String message = createPacket(SENDER_2_ID, "ds18b20", String(temp_val));

    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(100);  // Small delay to avoid excessive looping
}


float readTemperature() {
  float temp_val = sensors.getTempCByIndex(0); // Convert ADC value to equivalent voltage
  return temp_val;
}


String createPacket(uint8_t senderID, const String& sensorType, const String& sensorValue) {
  // Create a packet in the format: [SenderID:SensorType:SensorValue]
  String packet = "[" + String(senderID) + ":" + sensorType + ":" + sensorValue + "]";
  return packet;
}
