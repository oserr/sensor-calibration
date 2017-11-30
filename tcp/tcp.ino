/*
 WiFiEsp example: TCPExample

 This sketch connects to the lab PowerDue wifi and establishes
 a TCP connection with the Orchestra server. It also registers the device
 with a random id.
*/

#include "WiFiEsp.h"

char ssid[] = "PowerDue";        // your network SSID (name)
char pass[] = "powerdue";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


char server[] = "10.200.8.13";    // the IP address of the lab server
uint16_t port = 3000;             // port of the TCP server to connect to

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  SerialUSB.begin(115200);
  
  // initialize serial for ESP module
  // The ESP module works best at 9600 baud. By default its 115200.
  // 9600 is found to be more stable than at higher rates.
  // Make sure to set the ESP to this baud by sending
  // AT+UART_DEF=9600,8,1,0,0 to Serial1.
  Serial1.begin(9600);
  
  // initialize ESP module
  WiFi.init(&Serial1);
  
  // reset the ESP module
  WiFi.reset();

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    SerialUSB.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  
  // disconnect from any previous networks
  WiFi.disconnect();

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED ) {
    SerialUSB.print("Attempting to connect to WPA SSID: ");
    SerialUSB.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  SerialUSB.println("You're connected to the network");
  
  printWifiStatus();

  SerialUSB.println();
  SerialUSB.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, port)) {
    SerialUSB.println("Connected to server");
    // submits the device ID to the server
    client.println("github_id"); // must use println to auto append a '\n'
  }
}

void loop()
{
  // set up a serial pass through between this device and the server
  if(client.available()){
    // read a byte and write it to USB
    SerialUSB.write(client.read());
  }
  
  if(SerialUSB.available()){
    // read a byte and send it to the server
    client.write(SerialUSB.read());
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  SerialUSB.print("SSID: ");
  SerialUSB.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  SerialUSB.print("IP Address: ");
  SerialUSB.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  SerialUSB.print("Signal strength (RSSI):");
  SerialUSB.print(rssi);
  SerialUSB.println(" dBm");
}
