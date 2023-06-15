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

        // Print received XBee address
        XBeeAddress64 senderAddr = rx.getRemoteAddress64();
        Serial.print("Received from XBee address: ");
        Serial.print(senderAddr.getMsb(), HEX);
        Serial.print(" ");
        Serial.println(senderAddr.getLsb(), HEX);

        // Process received data packet
        String packet = parsePacket(rx.getData(), rx.getDataLength());

        // Get the sender ID from the packet
        uint8_t senderID = getSenderID(packet);

        // Add packet to the corresponding sender's queue
        if (senderID == 1) {
          enqueuePacket(packet, packetQueue1, queueFront1, queueRear1);
        } else if (senderID == 2) {
          enqueuePacket(packet, packetQueue2, queueFront2, queueRear2);
        } else if (senderID == 3) {
          enqueuePacket(packet, packetQueue3, queueFront3, queueRear3);
        } else if (senderID == 4) {
          enqueuePacket(packet, packetQueue4, queueFront4, queueRear4);
        }
      }
    }
  }

  // Process the packet queues sequentially
  processPacketQueue(packetQueue1, queueFront1, queueRear1);
  processPacketQueue(packetQueue2, queueFront2, queueRear2);
  processPacketQueue(packetQueue3, queueFront3, queueRear3);
  processPacketQueue(packetQueue4, queueFront4, queueRear4);

  delay(500);  // Add a short delay between each check
}

String parsePacket(uint8_t* data, uint8_t length) {
  String packet = "";

  // Convert received data bytes to a string
  for (int i = 0; i < length; i++) {
    packet += (char)data[i];
  }

  return packet;
}

uint8_t getSenderID(const String& packet) {
  // Extract the sender ID from the packet
  uint8_t senderID = packet.substring(1, packet.indexOf(':')).toInt();
  return senderID;
}

void enqueuePacket(const String& packet, Packet* queue, uint8_t& queueFront, uint8_t& queueRear) {
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
