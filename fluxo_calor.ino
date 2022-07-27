#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT
#include <OneWire.h> 
#include <DallasTemperature.h> //Lib DS18B20


#define FIREBASE_HOST "https://cacador-ifsc.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 120000 //Intervalo de 2 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "VETORIAL_356"
#define PASSWORD "50599882"

//Objeto que realiza a leitura da umidade e temperatura
SimpleDHT11 dht;
//Variáveis do sensor DHT
float temperature, humidity;

//variáveis UV Sensor
#define uvSensor 32
int indexUV;
int readUVSensor();

//Conexao Barramento DS18B20
#define fluxoCalor 15
OneWire oneWire(fluxoCalor);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress ds_1= { 0x28, 0xD5, 0xFC, 0xCD, 0x04, 0x00, 0x00, 0x8C };
DeviceAddress ds_2 = { 0x28, 0x63, 0xD6, 0x27, 0x00, 0x00, 0x80, 0x3C };
float heatflow = 0;
const float K = 0.1667663; //Constante da barra utilizada na montagem dos sensores
float ds_up, ds_down, flow;

//valor referente a um timestemp prox a leitura(01/10/2020 14:20:00)
uint32_t lastTimeRead = 1601572724;

void setup() {
  Serial.begin(115200);
  //Inicializa a conexão com a rede WiFi e Captura DataTime
  setupWiFi(); 
  // Start up the library    
  //Inicializa a lib do Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
//  sensors.setResolution(ds_1, 15);
//  sensors.setResolution(ds_2, 15);

}

void loop() {
  
  //Tempo em millissegundos desde o boot do esp
  unsigned long now = millis();
  //Se passou o intervalo desde a última leitura
  if (now - lastTimeRead > INTERVAL) {
    indexUV = readUVSensor();       
    readDht();
    heatflow = calculateHeatFlow();
    sendToFirebase();
    lastTimeRead = now;
  }
}

/*############ CONEXAO COM A REDE WIFI - PERSISTENCIA ############
  Conexão com a rede WiFi
  usando reconnect() para reconectar à rede em caso de perda do sinal
*/

void setupWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  //Tenta conectar à rede que possui este SSID e senha
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");

  //Enquanto não estiver conectado à rede WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Se chegou aqui está conectado
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/*############ SENSORES IMPLEMENTADOS NA ESTACAO DE MONITORAMENTO ############
  1. DHT11 - leitura e chamada da biblioteca
  2. UV - retorna um valor inteiro referente ao index
  3. Fluxo de calor - retorna um valor float com base nas duas temperaturas
*/

//SENSOR DHT 11
void readDht() {
  float t, h;
  //Coloca o valor lido da temperatura em t e da umidade em h
  int status = dht.read2(DHTPIN, &t, &h, NULL);
  //Se a leitura foi bem sucedida
  if (status == SimpleDHTErrSuccess) {
    //Os valores foram lidos corretamente, então é seguro colocar nas variáveis
    temperature = t;
    humidity = h;
  }
}

//SENSOR UV
int readUVSensor(void){
  int index, leitura, tensao;
    
  leitura = analogRead(uvSensor);
  tensao = (leitura*(3.3 / 4095))*1000;

  if(tensao <= 50){
    index = 0;
  }else if (tensao > 50 && tensao <=227){
    index = 1;
  }else if (tensao > 227 && tensao <=318){
    index = 2;
  }else if (tensao > 318 && tensao <=408){
    index = 3;
  }else if (tensao > 408 && tensao <=503){
    index = 4;
  }else if (tensao > 503 && tensao <=606){
    index = 5;
  }else if (tensao > 606 && tensao <=696){
    index = 6;
  }else if (tensao > 696 && tensao <=795){
    index = 7;
  }else if (tensao > 795 && tensao <=880){
    index = 8;
  }else if (tensao > 880 && tensao <=976){
    index = 9;
  }else if (tensao > 976 && tensao <=1079){
    index = 10;
  }else if (tensao > 1079 && tensao <=1170){
    index = 11;
  }else if (tensao > 1170){
    index = 12;
  }  
  return index;
}

float calculateHeatFlow(void){     
  sensors.requestTemperatures();
  //usando DS 1 EM CIMA
  ds_up = sensors.getTempC(ds_1);
  Serial.print("Sensor 1:  ");
  Serial.print(ds_up);
  Serial.print("  ºC  ");
    
  //usando DS 2 EM BAIXO
  ds_down = sensors.getTempC(ds_2);
  Serial.print("Sensor 2:  ");
  Serial.print(ds_down);
  Serial.print("  ºC  ");

  /* calculando o fluxo de calor
   *  + significa que o fluxo de calor está penetrando ao solo
   *  - significa que o fluxo de calor está saindo do solo
   */
  flow =((ds_up - ds_down)*K);
  Serial.print("Fluxo de calor:  ");
  Serial.print(flow);
  Serial.print("  W ");
  Serial.println();

  return flow;
  
}

/*############ ENVIA OS VALORES PARA O BANCO DE DADOS - FIREBASE ############
  1. Data e Hora (timestamp)  
  2. DHT11 - Temperatura e Umidade
  3. UV - Index
  4. Fluxo de calor 
*/
//Envia a temperatura e umidade para o Firebase
void sendToFirebase() {
  //Buffer para a criação dos jsons
  StaticJsonBuffer<150> jsonBufferSensor; 
  StaticJsonBuffer<50> jsonBufferTimestamp;
  //Criamos o objeto json que será enviado
  JsonObject& dados = jsonBufferSensor.createObject();   
  //Criamos o objeto json referente ao timestamp (Json auxiliar)
  JsonObject& timestamp = jsonBufferTimestamp.createObject();
  //Marcamos como um valor que o server do Firebase irá preencher com o timestamp
  timestamp[".sv"] = "timestamp"; 
  
  //pacote enviado ao BD
  
  dados["T"] = temperature; //Inserimos os atributo de temperatura
  dados["H"] = humidity; //Inserimos os atributo de umidade
  //dados["UV"] = indexUV; //indice UV
  dados["QS"] = heatflow; //fluxo de calor
  dados["MS_J"] = timestamp; //Inserimos o json referente ao timestamp no json que será enviado
  
  //Enviamos o json sensorData para o Firebase no path /History
  Firebase.push("/ESTACAO_01", dados);
  //Limpamos os buffers dos jsons
  jsonBufferTimestamp.clear();
  jsonBufferSensor.clear();
}
