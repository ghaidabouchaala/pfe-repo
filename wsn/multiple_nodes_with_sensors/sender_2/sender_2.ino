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
unsigned long transmissionInterval = 2000;  // Time interval between sending data (5 seconds)

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
    // Create message
    String message = "ds18b20 :" + String(sensors.getTempCByIndex(0));

    Serial.println(message);// Print temperature from Dallas DS18B20 sensor

    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(5000);  // Small delay to avoid excessive looping
}
