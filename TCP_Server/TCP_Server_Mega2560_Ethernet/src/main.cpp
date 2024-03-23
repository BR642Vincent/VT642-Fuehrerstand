#include "TCP_Config.h"

// definiere Pins als Ausgänge
void setOutputs()
{
  pinMode(SIFA, OUTPUT);
  pinMode(TRAKTIONSSPERRE, OUTPUT);
}

void tcpError()
{
  if (!client.connected())
  {
    Serial.println("TCP-Server nicht mehr verbunden, beende Code");
    client.stop();
    for (;;)
      ;
  }
}

void setup()
{
  setOutputs();

  Ethernet.init(CS_PIN);
  Ethernet.begin(mac);

  Serial.begin(9600);
  Serial.print("IP: "), Serial.println(Ethernet.localIP());

  delay(100);

  // TCP-Server starten
  if (client.connect(tcp_server_ip, PORT))
  {
    Serial.println("TCP-Server verbunden");
    tcp_zustand = TCP_SENDE_KONFIGURATION;
  }
  else
  {
    Serial.println("TCP-Server nicht verbunden");
  }

  tcp_zustand = TCP_VERBINDE;
}

void loop()
{
  switch (tcp_zustand)
  {
  case TCP_VERBINDE:
    Serial.println("TCP_VERBINDE");

    tcp_anmelde_daten[0] = TCP_VERBINDE_BYTE_ANZAHL - 4; // Paketlänge (4 byte)
    tcp_anmelde_daten[1] = 0x00;
    tcp_anmelde_daten[2] = 0x00;
    tcp_anmelde_daten[3] = 0x00;
    tcp_anmelde_daten[4] = 0x00; // HELLO (2 byte)
    tcp_anmelde_daten[5] = 0x01;
    tcp_anmelde_daten[6] = 0x01;  // Version (1 byte) DEC
    tcp_anmelde_daten[7] = 0x02;  // Client type (1 byte) DEC
    tcp_anmelde_daten[8] = 0x12;  // Identifikationslänge (1 byte) TCP_VERBINDE_BYTE_ANZAHL - 9 (gesamte Bytes - bisher verwendete bytes 27 - 9 = 18) DEC
    tcp_anmelde_daten[9] = 0x46;  // F ASCII
    tcp_anmelde_daten[10] = 0x61; // a ASCII
    tcp_anmelde_daten[11] = 0x68; // h ASCII
    tcp_anmelde_daten[12] = 0x72; // r ASCII
    tcp_anmelde_daten[13] = 0x70; // p ASCII
    tcp_anmelde_daten[14] = 0x75; // u ASCII
    tcp_anmelde_daten[15] = 0x6C; // l ASCII
    tcp_anmelde_daten[16] = 0x74; // t ASCII
    tcp_anmelde_daten[17] = 0x20; //   ASCII
    tcp_anmelde_daten[18] = 0x28; // ( ASCII
    tcp_anmelde_daten[19] = 0x41; // A ASCII
    tcp_anmelde_daten[20] = 0x72; // r ASCII
    tcp_anmelde_daten[21] = 0x64; // d ASCII
    tcp_anmelde_daten[22] = 0x75; // u ASCII
    tcp_anmelde_daten[23] = 0x69; // i ASCII
    tcp_anmelde_daten[24] = 0x6E; // n ASCII
    tcp_anmelde_daten[25] = 0x6F; // o ASCII
    tcp_anmelde_daten[26] = 0x29; // ) ASCII

    client.write(tcp_anmelde_daten, TCP_VERBINDE_BYTE_ANZAHL);

    tcp_zustand = TCP_WARTE_AUF_VERBINDUNG;
    break;

  case TCP_WARTE_AUF_VERBINDUNG:
    Serial.println("TCP_WARTE_AUF_VERBINDUNG");

    if (client.available())
    {
      tcp_zustand = TCP_SENDE_KONFIGURATION;
    }
    else
    {
      tcpError();
    }
    break;

  case TCP_SENDE_KONFIGURATION:
    Serial.println("TCP_SENDE_KONFIGURATION");

    /*Finger weg!!!*/
    tcp_konfigurations_daten[0] = TCP_KONFIGURATION_BYTE_ANZAHL - 4; // Paketlänge (4 byte)
    tcp_konfigurations_daten[1] = 0x00;
    tcp_konfigurations_daten[2] = 0x00;
    tcp_konfigurations_daten[3] = 0x00;
    tcp_konfigurations_daten[4] = 0x00; // Benötigte Daten (2 byte)
    tcp_konfigurations_daten[5] = 0x03;
    tcp_konfigurations_daten[6] = 0x00; // Befehlsvorrat (2 byte)
    tcp_konfigurations_daten[7] = 0x0A;

    /*Eigene Änderungen ab hier eintragen /  TCP_KONFIGURATION_BYTE_ANZAHL anpassen! / Befehle aus TCP_Config.h entnehmen*/
    tcp_konfigurations_daten[8] = 37; // Leuchtmelder SIFA
    tcp_konfigurations_daten[9] = 38; // LM Traktionssperre

    client.write(tcp_konfigurations_daten, TCP_KONFIGURATION_BYTE_ANZAHL);

    tcp_zustand = TCP_WARTE_AUF_KONFIGURATION;

  case TCP_WARTE_AUF_KONFIGURATION:
    Serial.println("TCP_WARTE_AUF_KONFIGURATION");

    if (client.available())
    {
      tcp_zustand = TCP_SENDE_KONFIGURATION_ENDE;
    }
    else
    {
      tcpError();
    }
    break;

  case TCP_SENDE_KONFIGURATION_ENDE:
    Serial.println("TCP_SENDE_KONFIGURATION_ENDE");

    tcp_konfigurations_ende_daten[0] = TCP_KONFIGURATION_ENDE_BYTE_ANZAHL - 4; // Paketlänge (4 byte)
    tcp_konfigurations_ende_daten[1] = 0x00;
    tcp_konfigurations_ende_daten[2] = 0x00;
    tcp_konfigurations_ende_daten[3] = 0x00;
    tcp_konfigurations_ende_daten[4] = 0x00; // Ende benötigte Daten (2 byte)
    tcp_konfigurations_ende_daten[5] = 0x03;
    tcp_konfigurations_ende_daten[6] = 0x00; // Befehlsvorrat (2 byte)
    tcp_konfigurations_ende_daten[7] = 0x00;

    client.write(tcp_konfigurations_ende_daten, TCP_KONFIGURATION_ENDE_BYTE_ANZAHL);

    tcp_zustand = TCP_WARTE_AUF_KONFIGURATION_ENDE;
    break;

  case TCP_WARTE_AUF_KONFIGURATION_ENDE:
    Serial.println("TCP_WART_AUF_KONFIGURATION_ENDE");

    if (client.available())
    {
      tcp_zustand = TCP_EMPFANGE_DATEN;
    }
    else
    {
      tcpError();
    }
    break;

  case TCP_EMPFANGE_DATEN:
    Serial.println("TCP_EMPFANGE_DATEN");

    if (client.available())
    {
      char paketlaenge[4];

      for (int p_byte = 0; p_byte < 4; p_byte++)
      {
        paketlaenge[p_byte] = client.read();
      }

      unsigned long *lval = (unsigned long *)paketlaenge;
      unsigned long paketinhalt_laenge = *lval;

      char paketinhalt[paketinhalt_laenge];

      for (int p_byte = 0; p_byte < paketinhalt_laenge; p_byte++)
      {
        paketinhalt[p_byte] = client.read();
      }
      tcp_zustand = TCP_WERTE_DATEN_AUS;
    }
    else
    {
      tcpError();
    }
    break;

  case TCP_WERTE_DATEN_AUS:
    Serial.println("TCP_WERTE_DATEN_AUS");

    if (client.available())
    {
      char paketlaenge[4];
      for (int p_byte = 0; p_byte < 4; p_byte++)
      {
        paketlaenge[p_byte] = client.read();
      }

      unsigned long *lval = (unsigned long *)paketlaenge;
      unsigned long paketinhalt_laenge = *lval;

      char paketinhalt[paketinhalt_laenge];

      for (int p_byte = 0; p_byte < paketinhalt_laenge; p_byte++)
      {
        paketinhalt[p_byte] = client.read();
      }

      int befehle = (paketinhalt_laenge - 2) / 5;

      if (paketinhalt_laenge > 2)
      {
        if (sizeof(paketinhalt) > 3)
        {
          int x = 2;
          int y = 3;

          for (int i = 0; i < befehle; i++)
          {
            Serial.print("Befehl: "), Serial.print(paketinhalt[x], DEC), Serial.print("Wert: ");

            float singlewert = *((float *)&paketinhalt[y]);
            int intwert = *((int *)&paketinhalt[y]);

            y += 4;

            switch (paketinhalt[x])
            {
            case 37:
              Serial.println(singlewert);
              digitalWrite(SIFA, singlewert != 0);
              break;
            case 38:
              Serial.println(singlewert);
              digitalWrite(TRAKTIONSSPERRE, singlewert != 0);
              break;
            }
            x = x + 5;
            y = y + 1;
          }
          Serial.println();
        }
        else
        {
          Serial.println();
        }
      }
    }
    else
    {
      tcpError();
    }
    break;
  }
}