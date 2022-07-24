#include <OneWire.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>



// --- Constantes ---
const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int delaytime = 1000;            //Invervalo entre as amostras (miliseconds)
int radius = 147;                //Raio do anemometro(mm)

//DATALOGGER SHIELD
int Pino_CS = 10;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
RTC_DS1307 rtc;
File ajuste;
File salvar;

// --- Variáveis Globais ---
unsigned int counter = 0;        //Contador para o sensor
unsigned int RPM = 0;            //Rotações por minuto
float windspeed = 0;             //Velocidade do vento (m/s)


// Definições Biruta
int dirpin=A1;
String direcao;
float leiturabrt =0;
int Winddir =0;
float tempC = 0;



// --- Configurações Inicias ---
void setup() {
  pinMode(2, INPUT);        //configura o digital 2 como entrada
  digitalWrite(2, HIGH);    //internall pull-up active
  Serial.begin(9600);       //inicia serial em 9600 baud rate

     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          //Inicia o cartao SD
//            Serial.println("Iniciando cartao SD...");
//            if (!SD.begin(Pino_CS))
//            {
//              Serial.println("Falha na inicializacao do SD!");
//              return;
//            }
//            Serial.println("Cartao SD iniciado. OK");
//            Serial.println();
          
             if (! rtc.begin()) {
              Serial.println("Couldn't find RTC");
              while (1);
            }
            if (! rtc.isrunning()) {
              Serial.println("RTC is NOT running!");
           
              rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
              //rtc.adjust(DateTime(2018, 1, 12, 3, 0, 0));
            }
     //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
} //end setup

// --- Loop Infinito ---
void loop() {
  windvelocity();
  RPMcalc(); 
  WindSpeed();    
  WindDir();    
                          DateTime now = rtc.now();
                          Serial.print(now.day(), DEC);
                          Serial.print('/');
                          Serial.print(now.month(), DEC);
                          Serial.print('/');
                          Serial.print(now.year(), DEC);
                          Serial.print(" ( ");
                          Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
                          Serial.print(" ) ");
                          Serial.print(now.hour(), DEC);
                          Serial.print(':');
                          Serial.print(now.minute(), DEC);
                          Serial.print(':');
                          Serial.print(now.second(), DEC); //salvei data e hora
                          
                          //dados a serem salvos
                          Serial.print("    Vel. Vento:  ");
                          Serial.print(windspeed);
                          Serial.print(" [m/s] ");
                          Serial.print("     Direcao a :");
                          Serial.print(Winddir);
                          Serial.print(" graus   >>      ");
                          Serial.print(direcao);
                          Serial.println();                                      

    
                      
//--------------   GRAVAÇÃO DOS DADOS COM O SHIELD DATALOGGER   --------------
//                              DateTime now = rtc.now();
//                              ajuste.print("Data/hora: ");
//                              ajuste.print(now.day() < 10 ? "0" : "");
//                              ajuste.print(now.day(), DEC);
//                              ajuste.print('/');
//                              ajuste.print(now.month() < 10 ? "0" : "");
//                              ajuste.print(now.month(), DEC);
//                              ajuste.print('/');
//                              ajuste.print(now.year(), DEC);
//                              ajuste.print(' ');
//                              ajuste.print(now.hour() < 10 ? "0" : "");
//                              ajuste.print(now.hour(), DEC);
//                              ajuste.print(':');
//                              ajuste.print(now.minute() < 10 ? "0" : "");
//                              ajuste.print(now.minute(), DEC);
//                              ajuste.print(':');
//                              ajuste.print(now.second() < 10 ? "0" : "");
//                              ajuste.print(now.second(), DEC);
//                              ajuste.print(' ');
          
          
                      //Arquivo para gravar os dados        
//                    File salvar = SD.open("FUCK.txt", FILE_WRITE); 
//                      if (salvar)
//                        {
//                    
//                          salvar.print(now.day(), DEC);
//                          salvar.print('/');
//                          salvar.print(now.month(), DEC);
//                          salvar.print('/');
//                          salvar.print(now.year(), DEC);
//                          salvar.print(" ( ");
//                          salvar.print(daysOfTheWeek[now.dayOfTheWeek()]);
//                          salvar.print(" ) ");
//                          salvar.print(now.hour(), DEC);
//                          salvar.print(':');
//                          salvar.print(now.minute(), DEC);
//                          salvar.print(':');
//                          salvar.print(now.second(), DEC); //salvei data e hora
//                          
//                          //dados a serem salvos
//                          salvar.print("    Vel. Vento:  ");
//                          salvar.print(windspeed);
//                          salvar.print(" [m/s] ");
//                          salvar.print("     Direcao a :");
//                          salvar.print(Winddir);
//                          salvar.print(" graus   >>      ");
//                          salvar.print(direcao);
//                          salvar.println();                                      
//                          salvar.close();
//                         
//                        }else{
//                               // Mensagem de erro caso ocorra algum problema
//                               // na abertura do arquivo
//                               Serial.println("Erro ao abrir arquivo.txt !");
//                             }       
                    
          //--------------   GRAVAÇÃO DOS DADOS COM O SHIELD DATALOGGER   --------------
    
  delay(delaytime); //taxa de atualização
} //end loop

//*******FUNÇÕES ESPECÍFICAS PARA CADA APLICAÇÃO UTILIZADA*******

    //Função para indicar a direção do vento
      void WindDir(){
        leiturabrt = analogRead(dirpin);
         if (leiturabrt>= 744 && leiturabrt <= 805) { 
          Winddir = 360;
          direcao= "NORTE";
          }else if (leiturabrt>= 806 && leiturabrt <= 885) { 
          Winddir = 337,5;
          direcao = "Norte-Noroeste";
          }else if(leiturabrt >= 433 && leiturabrt <= 529) {
          Winddir = 315;
          direcao = "Noroeste";
          }else if (leiturabrt>= 346 && leiturabrt <= 432) { 
          Winddir = 292,5;
          direcao = "Oeste-Noroeste";
          }else if (leiturabrt>= 886 && leiturabrt <= 961) { 
          Winddir = 270;
          direcao = "Oeste";
          }else if (leiturabrt>= 530 && leiturabrt <= 614) { 
          Winddir = 247,5;
          direcao= "Oeste-Sudoeste";
          }else if (leiturabrt>= 615 && leiturabrt <= 665) { 
          Winddir = 225;
          direcao = "Sudoeste";
          }else if (leiturabrt>= 214 && leiturabrt <= 264) {  
          Winddir = 202,5;
          direcao = "Sul-Sudoeste";
          }else if (leiturabrt>= 265 && leiturabrt <= 345) { 
          Winddir = 180;
          direcao= "SUL";
          }else if (leiturabrt>= 110 && leiturabrt <= 155) {  
          Winddir = 157,5;
          direcao = "Sul-Sudeste";
          }else if (leiturabrt>= 156 && leiturabrt <= 213) { 
          Winddir = 135;
          direcao = "Sudeste";
          }else if (leiturabrt>= 0 && leiturabrt <= 74) { 
          Winddir = 112,5;
          direcao = "Leste-Sudeste";
          }else if (leiturabrt>= 88 && leiturabrt <= 109) { 
          Winddir = 90;
          direcao = "LESTE";
          }else if (leiturabrt>= 75 && leiturabrt <= 87) {  
          Winddir = 67,5;
          direcao = "Leste-Nordeste";
          }else if (leiturabrt>= 433 && leiturabrt <= 529) {  
          Winddir = 45;
          direcao = "Nordeste";
          }else if (leiturabrt>= 346 && leiturabrt <= 432) { 
          Winddir = 22,5;
          direcao = "Norte-Nordeste";
          }       
          else {  
          Winddir = 000;
          }
       }

        //Função para medir velocidade do vento
        void windvelocity() {
          windspeed = 0;       
          counter = 0;
          attachInterrupt(0, addcount, RISING);
          unsigned long millis();
          long startTime = millis();
          while (millis() < startTime + period) {}
        }

          //Função para calcular o RPM
          void RPMcalc() {
            RPM = ((counter) * 60) / (period / 1000); // Calculate revolutions per minute (RPM)
          }

            //Velocidade do vento em m/s
            void WindSpeed() {
              windspeed = ((4 * pi * radius * RPM) / 60) / 1000; //Calcula a velocidade do vento em m/s
            } //end WindSpeed

              //Incrementa contador
              void addcount() {
                counter++;
              }

  
  
