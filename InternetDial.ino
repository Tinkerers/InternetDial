/*
 This sketch sends a hard-coded UDP get request to a pfSense firewall
 to retrieve the octet count on a specific interface. It then uses this
 value to determine the current speed of traffic on the interface and
 updates an attached Switec stepper motor module to display the value
 on a physical dial.
 
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu
#include <SwitecX25.h>

// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEPS (315*3)

// For motors connected to pins 3,4,5,6
SwitecX25 motor1(STEPS,4,5,6,7);

int poll_time = 1000;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xEE, 0x5D };

IPAddress serverIP(172, 16, 5, 1);
unsigned int serverPort = 161;
unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
byte packetBuffer[59]; //buffer to hold incoming packet,

// Yes we're aware of security concerns here. We've decided we don't care in
// this instance.
byte snmp_packet[] = {
0x30, 0x33, 0x02, 0x01, 0x01, 0x04, 0x0d, 0x31, 0x36, 0x31, 0x37, 0x63, 0x30, 0x77, 0x30, 0x72,
0x6b, 0x69, 0x6e, 0x67, 0xa0, 0x1f, 0x02, 0x04, 0x10, 0xde, 0x4d, 0x7e, 0x02, 0x01, 0x00, 0x02,
0x01, 0x00, 0x30, 0x11, 0x30, 0x0f, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x1f, 0x01, 0x01,
0x01, 0x0a, 0x05, 0x05, 0x00
};

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

uint32_t last_octets = 0;
unsigned long last_time = millis();

void setup() {
  Serial.begin(9600);

  // start the Ethernet and UDP:
  Ethernet.begin(mac);
  //Ethernet.localIP();
  Udp.begin(localPort);
  
  // run the motor against the stops
  motor1.zero();
}

uint32_t getInOctets() {
  // Send the packet
  Udp.beginPacket(serverIP, serverPort);
  int bytesSent = Udp.write(snmp_packet, sizeof(snmp_packet));
  Udp.endPacket();

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  uint32_t octets_in = 0;

  if(packetSize) {
    Serial.print("Received packet of size: ");
    Serial.println(packetSize);
    // read the packet into packetBufffer
    Udp.read(packetBuffer, packetSize);
    
    octets_in = ((uint32_t)packetBuffer[packetSize-4] << 24) + ((uint32_t)packetBuffer[packetSize-3] << 16)
            + ((uint32_t)packetBuffer[packetSize-2] << 8) + ((uint32_t)packetBuffer[packetSize-1]);
  } else {
    Serial.println("No packet received!");
  }
//  Udp.stop();
  return octets_in;
}

/* The motor won't move while we're in a delay(), so we have to
   take breaks from the delay to update the motor.
*/
void motor_delay(int ms) {
  for(int i=0; i < ms; i++) {
    delay(1);
    motor1.update();
  }
}

void loop() {
  uint32_t octets = getInOctets();
  uint32_t delta_kbytes = (octets - last_octets)/1024;
  unsigned long duration = millis() - last_time;
  last_octets = octets;
  last_time = millis();
  Serial.print("Octets: ");
  Serial.println(octets);
  Serial.print("delta kbytes: ");
  Serial.println(delta_kbytes);
  Serial.print("KBytes per second: ");
  unsigned long kbps = delta_kbytes / (duration/1000);
  Serial.println(kbps);
  
  unsigned long scaled = kbps * STEPS / 25000; // 7680 == 60Mbit/s
  Serial.print("scaled: ");
  Serial.println(scaled);

  
  motor1.setPosition(scaled);
  motor_delay(poll_time);
}
