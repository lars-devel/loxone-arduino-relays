/*
 UDPSendReceiveString

 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and receive messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */

#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Adafruit_PCF8574.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, xxx, xxx);

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

Adafruit_PCF8574 gpio0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("");
  Serial.println("Init Ethernet ...");

  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH Shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  Serial.println("Start Ethernet ...");

  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(100);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(100);                      // wait for a second
      //delay(1); // do nothing, no point running without Ethernet hardware
    }
  } else if (Ethernet.hardwareStatus() == EthernetW5100) {
    Serial.println("W5100 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5200) {
    Serial.println("W5200 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5500) {
    Serial.println("W5500 Ethernet controller detected.");
  } else {
    Serial.println("Ethernet started");
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  } else {
    Serial.println("Ethernet cable connected");
  }

  IPAddress subnet = Ethernet.subnetMask();
  Serial.print("Subnet: ");
  for (int i=0; i < 4; i++) {
    Serial.print(subnet[i], DEC);
    if (i < 3) {
      Serial.print(".");
    }
  }
  Serial.println("");

  // start UDP
  Udp.begin(localPort);

  Serial.println("UDP started");

  if (!gpio0.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8574");
  }
  gpio0.pinMode(0, OUTPUT);

  Serial.println("GPIO initialized");

  Serial.println("GPIO -> 1");
  gpio0.digitalWrite(0, HIGH);
  delay(1000);
  Serial.println("GPIO -> 0");
  gpio0.digitalWrite(0, LOW);
}

IPAddress remote_ip(192, 168, 178, 61);
unsigned int remotePort = 8888;      // local port to listen on

//ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBuffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    int bank = (int)packetBuffer[0] - '0';
    int port = (int)packetBuffer[2] - '0';
    int state = (int)packetBuffer[4] - '0';
    if ((bank >= 0) && (bank < 8) && (port >= 0) && (port < 8) && (state >= 0) && (state < 2)) {
      Serial.print("Bank ");
      Serial.print(bank);
      Serial.print(" Port ");
      Serial.print(port);
      Serial.print(" -> ");
      Serial.println(state ? "On" : "Off");
      gpio0.digitalWrite(port, state ? HIGH : LOW);
      /*
      if (packetBuffer[4] == '0') {
        Serial.println("GPIO -> 0");
        gpio0.digitalWrite(0, LOW);
      } else if (packetBuffer[4] == '1') {
        Serial.println("GPIO -> 1");
        gpio0.digitalWrite(0, HIGH);
      }
      */
    } else {
      Serial.print("Invalid data: ");
      Serial.println(packetBuffer);
    }
//    // send a reply to the IP address and port that sent us the packet we received
//    Udp.beginPacket(Udp.remoteIP(), remotePort/*Udp.remotePort()*/);
//    Udp.write(ReplyBuffer);
//    Udp.endPacket();
  } /*else {
    Serial.println("Waiting...");
    Udp.beginPacket(remote_ip, remotePort);
    Udp.write("Waiting...\n");
    Udp.endPacket();
    delay(1000);
  }*/

  //ICMPEchoReply echoReply = ping(remote_ip, 4);
  //if (echoReply.status == SUCCESS) {
  //  Serial.println("verbunden");
  //} else {
  //  Serial.println("getrennt");
  //}
  delay(1);
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
