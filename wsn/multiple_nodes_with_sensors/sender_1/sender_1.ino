#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial xbeeSerial(2, 3);  // XBee communication pins (RX, TX)
XBee xbee = XBee();

const int lm35_pin = A1;	// LM35 O/P pin

// Receiver XBee address
XBeeAddress64 receiverAddr = XBeeAddress64(0x0013A200, 0x41957E70);
const uint8_t SENDER_1_ID = 1;

unsigned long lastTransmissionTime = 0;
unsigned long transmissionInterval = 4000;  // Time interval between sending data (5 seconds)

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor
  xbeeSerial.begin(9600);  // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Sender 1 node initialized");
}

void loop() {

  int temp_adc_val;
  float temp_val;
  // Check if enough time has passed since the last transmission
  if (millis() - lastTransmissionTime >= transmissionInterval) {

    temp_adc_val = analogRead(lm35_pin);	// Read temperature from LM35 sensor
    temp_val = (temp_adc_val * 4.88);	// Convert ADC value to equivalent voltage
    temp_val = (temp_val/10);	
    Serial.println(temp_val); // Print temperature from LM35
    // Create message
    String message = "lm35 :" + String(temp_val);

    // Create XBee transmission request
    ZBTxRequest tx = ZBTxRequest(receiverAddr, (uint8_t*)message.c_str(), message.length());

    xbee.send(tx);

    lastTransmissionTime = millis();  // Update the last transmission time
  }

  delay(100);  // Small delay to avoid excessive looping
}
