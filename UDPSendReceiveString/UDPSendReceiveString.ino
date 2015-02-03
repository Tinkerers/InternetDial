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

void setup() {
  Serial.begin(9600);

  // start the Ethernet and UDP:
  Ethernet.begin(mac);
  //Ethernet.localIP();
  Udp.begin(localPort);

  // Seed our random
  randomSeed(analogRead(0));
  //randNumber = random(1000, 30000);
  //Serial.print("randNumber: ");
  //Serial.println(randNumber);  
  //snmp_packet[24] = random(0, 127);
  //snmp_packet[25] = random(0, 127);
  //snmp_packet[26] = random(0, 127);
  //snmp_packet[27] = random(0, 127);

  Serial.println("sending snmp_packet: ");
  for (int i=0; i < sizeof(snmp_packet); i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(snmp_packet[i], HEX);
  }
  Serial.println("end of sent snmp_packet");

  // Send the packet
  Udp.beginPacket(serverIP, serverPort);
  int bytesSent = Udp.write(snmp_packet, sizeof(snmp_packet));
  Serial.print("sent bytes: ");
  Serial.println(bytesSent);
  Udp.endPacket();
}

void loop() {
 
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, packetSize);
    Serial.println("Contents:");
    for (int i=0; i < packetSize; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(packetBuffer[i], HEX);
    }
    Serial.println("end of received packet");
    
    
    
    uint32_t thirty_two;

    Serial.print("Error?: ");
    Serial.println(packetBuffer[30], HEX);
    Serial.print("Octets MSB hex: ");
    Serial.println(packetBuffer[packetSize-4], HEX);
    Serial.print("Octets MSB dec: ");
    Serial.println(((uint32_t)packetBuffer[packetSize-4] << 24));
    
    thirty_two = ((uint32_t)packetBuffer[packetSize-4] << 24) + ((uint32_t)packetBuffer[packetSize-3] << 16)
            + ((uint32_t)packetBuffer[packetSize-2] << 8) + ((uint32_t)packetBuffer[packetSize-1]);

    Serial.print("InOctets: ");
    Serial.println(thirty_two);
  }   
  delay(10);
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


