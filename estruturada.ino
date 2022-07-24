import class anemometro;

#include <OneWire.h>
#include <DallasTemperature.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>
#include <DHT.h>


//ConfiguraÃ§Ã£o dos sensores DS18B20
// Conectar o pino central dos sensores ao pino 4 do Arduino
#define ONE_WIRE_BUS 4

// Configurar uma instÃ¢ncia oneWire de comunicaÃ§Ã£o com qualquer dispositivo
OneWire oneWire(ONE_WIRE_BUS);

// Tranfere a referÃªncia oneWire para a temperatura de Dallas
DallasTemperature sensors(&oneWire);

DeviceAddress Solo_15cm = { 0x28, 0xC9, 0x97, 0x28, 0x00, 0x00, 0x80, 0x11 };   //EndereÃ§o localizado com outro programa
DeviceAddress Solo_30cm = { 0x28, 0x63, 0xD6, 0x27, 0x00, 0x00, 0x80, 0x3C };
DeviceAddress Solo_45cm = { 0x28, 0xD5, 0xFC, 0xCD, 0x04, 0x00, 0x00, 0x8C };

float tempK1 = 0;
float tempK2 = 0;
float tempK3 = 0;
float tempC1 = 0;
float tempC2 = 0;
float tempC3 = 0;

//Configurando o sensor de Temperatura e Umidade DHT11
#define DHTPIN A0 // Informa o pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11                                     
DHT dht(DHTPIN, DHTTYPE);
float humdht = 0;
float tempdht = 0;

//DATALOGGER SHIELD
int Pino_CS = 53;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
RTC_DS1307 rtc;
File ajuste;
File salvar;

// --- VariÃ¡veis Globais ---
unsigned int counter = 0;        //Contador para o sensor
unsigned int RPM = 0;            //RotaÃ§Ãµes por minuto
float windspeed = 0;             //Velocidade do vento (m/s)



float tempC = 0;



// --- ConfiguraÃ§Ãµes Inicias ---
void setup() {
  pinMode(2, INPUT);        //configura o digital 2 como entrada
  digitalWrite(2, HIGH);    //internall pull-up active
  Serial.begin(9600);       //inicia serial em 9600 baud rate
  dht.begin();              //Inicia o sensor DHT11

  //*****************************************************************************************
  //Inicializando os DS18B20
  // Start a biblioteca DallasTemperature
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(Solo_15cm, 10);
  sensors.setResolution(Solo_30cm, 10);
  sensors.setResolution(Solo_45cm, 10);
  //*****************************************************************************************

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //Inicia o cartao SD
  Serial.println("Iniciando cartao SD...");
  if (!SD.begin(Pino_CS))
  {
    Serial.println("Falha na inicializacao do SD!");
    return;
  }
  Serial.println("Cartao SD iniciado. OK");
  Serial.println();

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
  //*************BARRAMENTO DS18B20*************
  sensors.requestTemperatures();
  tempC1 = sensors.getTempC(Solo_15cm);
  tempC2 = sensors.getTempC(Solo_30cm);
  tempC3 = sensors.getTempC(Solo_45cm);

  tempK1 = (sensors.getTempC(Solo_15cm) + 273);
  tempK2 = (sensors.getTempC(Solo_30cm) + 273);
  tempK3 = (sensors.getTempC(Solo_45cm) + 273);

  //*************BARRAMENTO DS18B20*************


  //######################################### SENSOR DHT11 #########################################

  humdht = dht.readHumidity();
  tempdht = dht.readTemperature();


  //--------------   GRAVAÃ‡ÃƒO DOS DADOS COM O SHIELD DATALOGGER   --------------
  DateTime now = rtc.now();
  ajuste.print("Data/hora: ");
  ajuste.print(now.day() < 10 ? "0" : "");
  ajuste.print(now.day(), DEC);
  ajuste.print('/');
  ajuste.print(now.month() < 10 ? "0" : "");
  ajuste.print(now.month(), DEC);
  ajuste.print('/');
  ajuste.print(now.year(), DEC);
  ajuste.print(' ');
  ajuste.print(now.hour() < 10 ? "0" : "");
  ajuste.print(now.hour(), DEC);
  ajuste.print(':');
  ajuste.print(now.minute() < 10 ? "0" : "");
  ajuste.print(now.minute(), DEC);
  ajuste.print(':');
  ajuste.print(now.second() < 10 ? "0" : "");
  ajuste.print(now.second(), DEC);
  ajuste.print(' ');


  //Arquivo para gravar os dados
  File salvar = SD.open("FUCK.txt", FILE_WRITE);
  if (salvar)
  {

    salvar.print(now.day(), DEC);
    salvar.print('/');
    salvar.print(now.month(), DEC);
    salvar.print('/');
    salvar.print(now.year(), DEC);
    salvar.print(" ( ");
    salvar.print(daysOfTheWeek[now.dayOfTheWeek()]);
    salvar.print(" ) ");
    salvar.print(now.hour(), DEC);
    salvar.print(':');
    salvar.print(now.minute(), DEC);
    salvar.print(':');
    salvar.print(now.second(), DEC); //salvei data e hora

    //dados a serem salvos
    salvar.print("      Umidade: ");
    salvar.print(humdht);
    salvar.print(" %  ");
    salvar.print("     Temperatura: ");
    salvar.print(tempdht);
    salvar.println(" *C");
    salvar.println();
    salvar.print("    Vel. Vento:  ");
    salvar.print(windspeed);
    salvar.print(" [m/s] ");
    salvar.print("     Direcao a :");
    salvar.print(Winddir);
    salvar.print(" graus   >>      ");
    salvar.print(direcao);
    salvar.println();
    salvar.print("Temperatura Sensor 1: ");
    salvar.print(tempC1);
    salvar.print(" \r\r    Leitura em (K): ");
    salvar.println(tempK1);
    salvar.println();
    salvar.print("Temperatura Sensor 2: ");
    salvar.print(tempC2);
    salvar.print(" \r\r    Leitura em (K): ");
    salvar.println(tempK2);
    salvar.println();
    salvar.print("Temperatura Sensor 3: ");
    salvar.print(tempC3);
    salvar.print(" \r\r    Leitura em (K): ");
    salvar.println(tempK3);
    salvar.println();
    salvar.close();

  } else {
    // Mensagem de erro caso ocorra algum problema
    // na abertura do arquivo
    Serial.println("Erro ao abrir arquivo.txt !");
  }

  //--------------   GRAVACAO DOS DADOS COM O SHIELD DATALOGGER   --------------

  delay(delaytime); //taxa de atualizacoes
} //end loop

//*******FUNÃ‡Ã•ES ESPECÃ�FICAS PARA CADA APLICAÃ‡ÃƒO UTILIZADA*******

//Funcao para ler temperatura dos sensores DS18B20
void tempCelsius(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
}
