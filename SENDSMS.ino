
#include <VirtualWire.h> // Inclui essa biblioteca
#include <GSM.h>
#define PINNUMBER ""
GSM gsmAccess;
GSM_SMS sms;

int pino_sensor = 5;
int pino_led1vermelho = 30;
int pinRele2 = 46;
int pino_led2verde = 50;
int pinRele1 = 40;
int alarme_disparado = 0;
long tempo_alarme = 0;

char remoteNumber[20];  // Mantém o número emissor
char message[180];

int x,startbit,ctr,dataok,t1,larpulso,larpulso1,larpulso2,larpulso3,larpulso4,bt1,bt2,antcode=0;
unsigned long _dur,_dur1;      // Duração do Pulso
unsigned long dado_rf=0;   // Dado é o Código recebio do HT6p20 todos os 28 BITS onde, 22 do coodigo+2 dos botões+4 do anti-codigo;
int  pino_rf=2;    // Pino que vai receber o sinal RF digital.


void setup()
{
  Serial.begin(9600);
  pinMode(pino_rf, INPUT);

  pinMode(pino_sensor,INPUT);
  digitalWrite(pino_sensor,HIGH); // PULL UP DO SENSOR
  pinMode(pino_led1vermelho,OUTPUT);
  pinMode(pinRele2,OUTPUT);
  pinMode(pino_led2verde,OUTPUT);
  pinMode(pinRele1,OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  boolean notConnected = true;
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
    }
  }
Serial.println("Loop");
}

void loop()
{
  le_codigo_rf();// ler codigo RF
  ler_sms(); // Veririca se algum SMS foi recebido

  if (alarme_disparado == 1) { 
    digitalWrite(pinRele1,HIGH); 
    digitalWrite(pino_led2verde,LOW); 
    alarme_disparado = 1;
    sms.beginSMS("99720331");
    sms.print("Alarme disparado!-SMS 1");
    sms.endSMS(); 
    tempo_alarme=millis();
    while (alarme_disparado != 0){
      digitalWrite(pino_led1vermelho,HIGH);
      delay(500);               
      digitalWrite(pino_led1vermelho, LOW);    
      delay(500); 
      ler_sms();
      if ((millis() > (tempo_alarme+6000)) && (alarme_disparado == 1)){
          sms.beginSMS("91391440");
		  sms.print("Alarme disparado!-SMS 2");
          sms.endSMS(); 
		  alarme_disparado = 2;
      }
      if ((millis() > (tempo_alarme+12000)) && (alarme_disparado == 2)){
		  sms.print("Alarme Desligado-Timeout");
		  alarme_disparado = 0; // para de tocar alarme
      }	  
    }
  }else{
  digitalWrite(pino_led2verde,HIGH); 
  digitalWrite(pinRele1,LOW); 
  }
}


void ler_sms()
{
  char c;
  char *p;
  char *q;
  char *r;
  char *s;
  int i;

  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println("SMS recebido do Celular:");
    // Get remote number
    sms.remoteNumber(remoteNumber, 20);
    Serial.println(remoteNumber);
    // Read message bytes and print them
    i=0;
    while(c=sms.read())
    {
      message[i++] = c;
      Serial.print(c);
    }
    message[i] = 0;

    // Verifica se a mensagem eh 123on  ou 123off
    p=strstr(message,"123alarmeoff");
    q=strstr(message,"123Alarmeoff");
    r=strstr(message,"123Lampadaon");
    s=strstr(message,"123Lampadaoff");
    if((p)||(q)){
      Serial.println("Comando Recebido: 123Alarme OFF");
      alarme_disparado=0;  
    }
    else  if(r){
      Serial.println("Comando Recebido: 123Lampada ON");
      digitalWrite(pinRele2,HIGH);
    }else if(s){
      Serial.println("Comando Recebido: 123Lampada OFF");
      digitalWrite(pinRele2,LOW);
    }else{
      Serial.println("Comando Invalido");
    }


    Serial.println("\nFim da Mensagem");

    // delete message from modem memory
    sms.flush();
    Serial.println("Mensagem  apagada");
  }
}


/*
  Read input serial
 */
int readSerial(char result[])
{
  int i = 0;
  while(1)
  {
    while (Serial.available() > 0)
    {
      char inChar = Serial.read();
      if (inChar == '\n')
      {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if(inChar!='\r')
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}

void le_codigo_rf(){
  digitalWrite(13, digitalRead(pino_rf)); //Pisca o LED conforme o valor da entrada digital, testar interferencias.
  if (startbit==0)
  {// Testa o tempo piloto até o Bit de inicio;
    _dur = pulseIn(pino_rf, LOW);
    if(_dur > 8000 && _dur < 12000 && startbit==0)
    {
      larpulso=_dur/23;
      larpulso1=larpulso-50;
      larpulso2=larpulso+50;
      larpulso3=larpulso+larpulso-50;
      larpulso4=larpulso+larpulso+50;
      startbit=1;
      _dur=0;
      dado_rf=0;
      dataok=0;
      ctr=0;
      // Se o Bit de inicio OK ENTÃO Inicia a medição do tempo em Nivel ALTO dos sinais, e testa se o tempo está na faixa.
      while (startbit==1 && dataok==0 && ctr < 28)
      {
        ++ctr;
        _dur1 = pulseIn(pino_rf, HIGH);
        if(_dur1 > larpulso1 && _dur1 < larpulso2)     // Se a largura de pulso é entre 1/4000 e 1/3000 segundos
        {
          dado_rf = (dado_rf << 1) + 1;      // anexar um * 1 * para a extremidade mais à direita do buffer
        }
        else if(_dur1 > larpulso3 && _dur1 < larpulso4)   // Se a largura de pulso é entre 2/4000 e 2/3000 segundos
        {
          dado_rf = (dado_rf << 1);       // anexar um * 0 * para a extremidade mais à direita do buffer
        }
        else
        {
          /* força finalização do laço */
          startbit=0;
        }

      }
      if (ctr==28)
      { 
        if (bitRead(dado_rf,0)==1)
        { 
          if (bitRead(dado_rf,1)==0)
          {
            if (bitRead(dado_rf,2)==1)
            {
              if (bitRead(dado_rf,3)==0)
              {
                antcode=1;
                // Se foram recebidos todos os 28 Bits, o valor vai para a variavel _data e pode ser usada como exemplo abaixo.
                dataok=1;
                Serial.print("Codigo Recebido:");
                Serial.println(dado_rf);

                if((dado_rf = 186534997)||(dado_rf = 189585301)) {
                  alarme_disparado=1;
                }
                ctr=0;
                startbit=0;
                antcode=0;
                delay(100);        
              }
            }
          }
        }        
      }              
    }
  }
}  
