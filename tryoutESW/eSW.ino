#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress arduinoIP(192, 168, 178, 111);
IPAddress piIP(192, 168, 178, 66);

EthernetClient client;

void setup() 
{
  Serial.begin(9600);
  
  if (Ethernet.begin(mac) == 0) 
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    
    // Fallback
    Ethernet.begin(mac, arduinoIP);
    Ethernet.setGatewayIP(IPAddress(192, 168, 178, 1));
    Ethernet.setSubnetMask(IPAddress(255, 255, 255, 0));
  }
  
  delay(1000);
}

void loop() 
{
  if (!client.connected()) 
  {
    Serial.println("Connecting to Raspberry Pi...");
    
    if (client.connect(piIP, 80)) 
    {
      Serial.println("Connected to Raspberry Pi");
      client.println("Hello, Raspberry Pi!");
      client.stop();
    } 
    else 
    {
      Serial.println("Connection to Raspberry Pi failed");
    }
    delay(5000);
  }
}