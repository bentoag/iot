
#include "etherShield.h"
#include "ETHER_28J60.h"

int led_pin = 7;

static uint8_t mac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};   // MAC-Adress - so pode existir um por REDE                                                                                                                           
static uint8_t ip[4] = {192, 168, 0, 55};                       // IP Adree - configurar conforme sua REDE
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
//    ether.print("<hml><head><title>Pagina Web</title></head><body>");
    ether.print("<H1>Arduino - Web Server</H1>");
    if (strcmp(params, "?cmd=on") == 0)
    {
      digitalWrite(led_pin, HIGH);
    }
    else if (strcmp(params, "?cmd=off") == 0)
    {
      digitalWrite(led_pin, LOW);
    }
    ether.print("<A HREF='?cmd=on'>Liga LED</A><p>");
    ether.print("<A HREF='?cmd=off'>Desliga LED</A><p>");
    ether.print("<br>");
    ether.print("<h2>Entrada Analogica A0=");
    ether.print(analogRead(A0));
    ether.print("</h2>");
//    ether.print("</body></html>");
    
    ether.respond();
  }
}

