#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

const int lm35_pin = A0;	// LM35 O/P pin

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_1_ID = 1;

unsigned long lastTransmissionTime = 0;
unsigned long transmissionInterval = 5000;  // Time interval between sending data (5 seconds)

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender 1 node initialized");
}

void loop() {

  // Check if enough time has passed since the last transmission
   if (millis() - lastTransmissionTime >= transmissionInterval) {
    float temp_val = readTemperature();

    // Create message packet
    String message = createPacket(SENDER_1_ID, "lm35", String(temp_val));

    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(100);  // Small delay to avoid excessive looping
}

float readTemperature() {
  int temp_adc_val = analogRead(lm35_pin);  // Read temperature from LM35 sensor
  float temp_val = (temp_adc_val * 4.88);  // Convert ADC value to equivalent voltage
  return (temp_val / 10);
}

String createPacket(uint8_t senderID, const String& sensorType, const String& sensorValue) {
  // Create a packet in the format: [SenderID:SensorType:SensorValue]
  String packet = "[" + String(senderID) + ":" + sensorType + ":" + sensorValue + "]";
  return packet;
}