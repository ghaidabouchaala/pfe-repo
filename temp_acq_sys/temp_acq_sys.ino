#include <OneWire.h>  // Library for the OneWire protocol
#include <DallasTemperature.h>  // Library for Dallas temperature sensors

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int lm35_pin = A1;  // LM35 Output pin

void setup() {
  Serial.begin(9600);  // Initialize serial communication for debugging
  sensors.begin();  // Initialize Dallas temperature sensors
}

void loop() {
  int temp_adc_val;
  float temp_val;
  
  sensors.requestTemperatures();  // Request temperature data from Dallas temperature sensor
  
  temp_adc_val = analogRead(lm35_pin);  // Read temperature from LM35 sensor
  temp_val = (temp_adc_val * 4.88);  // Convert ADC value to equivalent voltage
  temp_val = (temp_val/10);  // LM35 gives output of 10mV/°C
  Serial.print(temp_val);  // Print temperature from LM35
  Serial.println();  
  
  Serial.print(sensors.getTempCByIndex(0));  // Print temperature from Dallas DS18B20 sensor
  Serial.println();
  delay(1000);
}
