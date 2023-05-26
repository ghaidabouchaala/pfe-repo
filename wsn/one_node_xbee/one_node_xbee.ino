#include <SoftwareSerial.h>

SoftwareSerial mySerial (4,5); // 4 --> Tx 5--> R

void setup () {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop () {
  int myvalue=analogRead(A0) ;
  myvalue= myvalue*0.48828125;
  Serial.println(myvalue);

  int ch = 0x3FB + myvalue; 
  byte checksum = ch;
  checksum = 0xFF - checksum;
  // 00 13 A2 00 41 9в 54 08
  byte data [] = {0x7E, 0x00, 0x0F, 0x10, 0x01, 0x00, 0x13, 0x2, 0x00, 0x41, 0x9B, 0x54, 0x08, 0xFF, 0xFE, 0x00, 0x00, myvalue, checksum};

  mySerial.write(data, 19);
  delay(3500);
}