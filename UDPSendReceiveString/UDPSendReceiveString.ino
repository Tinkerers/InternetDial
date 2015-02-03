/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender
 
 A Processing sketch is included at the end of file that can be used to send 
 and received messages for testing with a computer.
 
 created 21 Aug 2010
 by Michael Margolis
 
 This code is in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

long randNumber;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xEE, 0x5D };

IPAddress serverIP(172, 16, 5, 1);
unsigned int serverPort = 161;

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
byte packetBuffer[57]; //buffer to hold incoming packet,

byte snmp_packet[] = {
0x30, 0x32, 0x02, 0x01, 0x01, 0x04, 
0x0d, 0x31, 0x36, 0x31, 0x37, 0x63, 0x30, 0x77, 0x30, 0x72, 0x6b, 0x69, 0x6e, 0x67, 0xa0, 0x1e, 
0x02, 0x04, 0x25, 0x17, 0x92, 0xce, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x10, 0x30, 0x0e, 
0x06, 0x0a, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x02, 0x02, 0x01, 0x0a, 0x05, 0x05, 0x00};

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

uint32_t last_octets = 0;

void setup() {
  Serial.begin(9600);

  // start the Ethernet and UDP:
  Ethernet.begin(mac);
  //Ethernet.localIP();
  Udp.begin(localPort);
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

void loop() {
  unsigned long start_ts = millis();
  uint32_t octets = getInOctets();
  uint32_t delta = octets - last_octets;
  unsigned long duration = millis() - start_ts;
  int poll_time = 1000;
  Serial.print("Octets: ");
  Serial.println(octets);
  Serial.print("delta: ");
  Serial.println(delta);
  Serial.print("KBytes per second: ");
  Serial.println((delta / duration) * 1000 / 8);
  delay(poll_time);
  last_octets = octets;
}


/*
  Processing sketch to run with this example
 =====================================================
 
 // Processing UDP example to send and receive string data from Arduino 
 // press any key to send the "Hello Arduino" message
 
 
 import hypermedia.net.*;
 
 UDP udp;  // define the UDP object
 
 
 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message  
 }
 
 void draw()
 {
 }
 
 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port
 
 udp.send("Hello World", ip, port );   // the message to send
 
 }
 
 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler
 
 for(int i=0; i < data.length; i++) 
 print(char(data[i]));  
 println();   
 }
 */


