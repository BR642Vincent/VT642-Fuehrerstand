#include "TCP_Config.h"

// definiere Pins als Ausgänge
void setOutputs()
{
  pinMode(TUER_RECHTS, OUTPUT);
  pinMode(TUER_LINKS, OUTPUT);
  pinMode(FEDERSPREICHER, OUTPUT);
}

void setup()
{
  setOutputs();

  Serial.begin(115200);

  // Verbindung mit WiFi starten
  Serial.println("WiFi starten...");

/*
  if (true)
  {
    Serial.print("WiFi-Konfiguration fehlgeschlagen");
  }
*/

  WiFi.begin(ssid, passwort);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("...");
  }

  Serial.println("WiFi verbunden");
  Serial.println(WiFi.localIP());

  delay(100);

  // TCP-Server starten
  if (client.connect(tcpServerIP, PORT))
  {
    Serial.println("TCP-Server verbunden");
  }
  else
  {
    Serial.println("TCP-Server nicht verbunden");

  }
}

void loop()
{
 
}
