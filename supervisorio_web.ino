
#include "etherShield.h"
#include "ETHER_28J60.h"

int led_pin = 6;

static uint8_t mac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};   // MAC-Adress - so pode existir um por REDE                                                                                                                           
static uint8_t ip[4] = {192, 168, 1, 15};                       // IP Adree - configurar conforme sua REDE
static uint16_t port = 80;                                      // PORTA 80 - Porta Padrao para HTTP

ETHER_28J60 ether;

void setup()
{ 
  ether.setup(mac, ip, port);
  pinMode(led_pin, OUTPUT);
}

void loop()
{
  char* params;
  if (params = ether.serviceRequest())
  {    
        // Cabecalho HTTP
        ether.print("HTTP/1.1 200 OK");
        ether.print("Content-Type: text/html");
        ether.print("Connection: close");
        ether.print("\n");
        ether.print("<!DOCTYPE HTML>");
        // Inicio da pagina HTML
        ether.print("<html><head><tile> Supervisorio Arduino </title><head><body>");
        // Atualizacao Automatica a CADA 5s
        ether.print("<meta http-equiv=\"refresh\" content=\"5\">");        
        // Dados do supervisorio
        ether.print("<h2>Entradas Analogicas</h2>");
        for (int analogChannel = 0; analogChannel < 6; analogChannel++)
        {
          int sensorReading = analogRead(analogChannel);
          ether.print("analog input ");
          ether.print(analogChannel);
          ether.print(" is ");
          ether.print(sensorReading);
          ether.print("<br />");       
        }
        // Final da Página HTML
        ether.print("</body></html>");
        ether.respond();
  }
}

