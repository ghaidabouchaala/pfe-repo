#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht11.h>

#define DHT11PIN 7

dht11 DHT11;

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int lm35_pin = A1;	// LM35 O/P pin
const int trigPin = 9;  
const int echoPin = 10; 
float duration, distance;  

void setup() {
  sensors.begin();
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);  
  Serial.begin(9600);  
}

void loop() {
  int temp_adc_val;
  float temp_val;
  
  int chk = DHT11.read(DHT11PIN); // Read data from DHT11 sensor
  
  sensors.requestTemperatures(); // Request temperature data from Dallas temperature sensor

  temp_adc_val = analogRead(lm35_pin);	// Read temperature from LM35 sensor
  temp_val = (temp_adc_val * 4.88);	// Convert ADC value to equivalent voltage
  temp_val = (temp_val/10);	
  Serial.print(temp_val); // Print temperature from LM35
  Serial.println();

  Serial.print(sensors.getTempCByIndex(0)); // Print temperature from Dallas DS18B20 sensor
  Serial.println();

  Serial.println((float)DHT11.humidity, 2); // Print humidity from DHT11 sensor
  Serial.println((float)DHT11.temperature, 2); // Print temperature from DHT11 sensor

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  Serial.println(distance); // Print distance from HC-SR04 ultrasonic sensor
  delay(1000);
}
