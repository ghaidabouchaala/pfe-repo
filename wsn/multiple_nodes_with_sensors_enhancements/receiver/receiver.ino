#include <XBee.h>

XBee xbee = XBee();
HardwareSerial& xbeeSerial = Serial1;  // Use Serial1 for XBee communication

const unsigned long receptionTimeout = 5000;  // Timeout for receiving a packet (in milliseconds)
const uint8_t MAX_QUEUE_SIZE = 10;  // Maximum number of packets to store in each sender's queue

struct Packet {
  String data;
};

Packet packetQueue1[MAX_QUEUE_SIZE];  // FIFO packet queue for sender 1
Packet packetQueue2[MAX_QUEUE_SIZE];  // FIFO packet queue for sender 2
Packet packetQueue3[MAX_QUEUE_SIZE];  // FIFO packet queue for sender 3
Packet packetQueue4[MAX_QUEUE_SIZE];  // FIFO packet queue for sender 4

uint8_t queueFront1 = 0;  // Index of the front of queue 1
uint8_t queueRear1 = 0;  // Index of the rear of queue 1
uint8_t queueFront2 = 0;  // Index of the front of queue 2
uint8_t queueRear2 = 0;  // Index of the rear of queue 2
uint8_t queueFront3 = 0;  // Index of the front of queue 3
uint8_t queueRear3 = 0;  // Index of the rear of queue 3
uint8_t queueFront4 = 0;  // Index of the front of queue 3
uint8_t queueRear4 = 0;  // Index of the rear of queue 3

void setup() {
  Serial.begin(9600);         // Initialize the serial monitor
  xbeeSerial.begin(9600);     // Set the baud rate for XBee communication
  xbee.setSerial(xbeeSerial);
  Serial.println("Receiver node initialized");
}

void loop() {
  if (xbeeSerial.available()) {
    xbee.readPacket();

    if (xbee.getResponse().isAvailable()) {
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        ZBRxResponse rx;
        xbee.getResponse().getZBRxResponse(rx);

        // Print received XBee address and data packet
        XBeeAddress64 senderAddr = rx.getRemoteAddress64();
        Serial.print("[");
        Serial.print(senderAddr.getMsb(), HEX);
        Serial.print(" ");
        Serial.print(senderAddr.getLsb(), HEX);
        Serial.print(":");

        // Process received data packet
        String packet = parsePacket(rx.getData(), rx.getDataLength());
        Serial.print(getSenderID(packet));
        Serial.print(":");
        Serial.print(getSensorType(packet));
        Serial.print(":");
        Serial.print(getSensorValue(packet));
        Serial.println("");

        // Get the sender ID from the packet
        uint8_t senderID = getSenderID(packet);

        // Add packet to the corresponding sender's queue
        enqueuePacket(packet, senderID);
      }
    }
  }

  // Process the packet queues sequentially
  processPacketQueue(packetQueue1, queueFront1, queueRear1);
  processPacketQueue(packetQueue2, queueFront2, queueRear2);
  processPacketQueue(packetQueue3, queueFront3, queueRear3);
  processPacketQueue(packetQueue4, queueFront4, queueRear4);

  delay(500);  // Add a short delay between processing loops
}

String parsePacket(uint8_t* data, uint8_t length) {
  String packet = "";

  // Convert the received data array to a string
  for (int i = 0; i < length; i++) {
    packet += (char)data[i];
  }

  // Add a new line character after the packet
  packet += "\n";

  return packet;
}

uint8_t getSenderID(const String& packet) {
  // Extract the sender ID from the packet
  uint8_t senderID = packet.substring(1, packet.indexOf(':')).toInt();
  return senderID;
}

void enqueuePacket(const String& packet, uint8_t senderID) {
  // Determine the corresponding sender's queue
  Packet* queue;
  uint8_t queueFront;
  uint8_t queueRear;

  if (senderID == 1) {
    queue = packetQueue1;
    queueFront = queueFront1;
    queueRear = queueRear1;
  } else if (senderID == 2) {
    queue = packetQueue2;
    queueFront = queueFront2;
    queueRear = queueRear2;
  } else if (senderID == 3) {
    queue = packetQueue3;
    queueFront = queueFront3;
    queueRear = queueRear3;
  } else if (senderID == 4) {
    queue = packetQueue4;
    queueFront = queueFront4;
    queueRear = queueRear4;
  } else {
    // Invalid sender ID, discard the packet
    return;
  }

  // Check if the queue is full
  if ((queueRear + 1) % MAX_QUEUE_SIZE == queueFront) {
    // Queue is full, discard the oldest packet
    queueFront = (queueFront + 1) % MAX_QUEUE_SIZE;
  }

  // Add the packet to the queue
  queue[queueRear].data = packet;
  queueRear = (queueRear + 1) % MAX_QUEUE_SIZE;
}

void processPacketQueue(Packet* queue, uint8_t& queueFront, uint8_t& queueRear) {
  // Process packets in the queue
  while (queueFront != queueRear) {
    // Get the front packet
    String packet = queue[queueFront].data;

    // Process the packet
    Serial.print("Sender ID: ");
    Serial.println(getSenderID(packet));
    Serial.print("Sensor Type: ");
    Serial.println(getSensorType(packet));
    Serial.print("Sensor Value: ");
    Serial.println(getSensorValue(packet));

    // Remove the front packet from the queue
    queueFront = (queueFront + 1) % MAX_QUEUE_SIZE;
  }
}

String getSensorType(const String& packet) {
  // Extract the sensor type from the packet
  int colonIndex1 = packet.indexOf(':');
  int colonIndex2 = packet.lastIndexOf(':');
  String sensorType = packet.substring(colonIndex1 + 1, colonIndex2);
  return sensorType;
}

String getSensorValue(const String& packet) {
  // Extract the sensor value from the packet
  int colonIndex2 = packet.lastIndexOf(':');
  String sensorValue = packet.substring(colonIndex2 + 1, packet.length() - 1);
  return sensorValue;
}
