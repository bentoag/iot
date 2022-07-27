#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT

#define FIREBASE_HOST "https://cacador-ifsc.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 600000 //Intervalo de 10 min entre cada leitura do sensor

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

//valor referente a um timestemp prox a leitura(01/10/2020 13:30:00)
uint32_t lastTimeRead = 0;

void setup() {
  Serial.begin(115200);
  //Inicializa a conexão com a rede WiFi e Captura DataTime
  setupWiFi(); 
  //Inicializa a lib do Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {
  
  //Tempo em millissegundos desde o boot do esp
  unsigned long now = millis();
  //Se passou o intervalo desde a última leitura
  if (now - lastTimeRead > INTERVAL) {
    indexUV = readUVSensor();       
    readDht();
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
  Serial.println("Tentando Conectar...");

  //Enquanto não estiver conectado à rede WiFi
  while (WiFi.status() != WL_CONNECTED) {    
    WiFi.reconnect();  
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


/*############ ENVIA OS VALORES PARA O BANCO DE DADOS - FIREBASE ############
  1. Data e Hora (timestamp)  
  2. DHT11 - Temperatura e Umidade
  3. UV - Index 
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
  dados["ms"] = timestamp; //Inserimos o json referente ao timestamp no json que será enviado
  dados["T"] = temperature; //Inserimos os atributo de temperatura
  dados["H"] = humidity; //Inserimos os atributo de umidade
  dados["UV"] = indexUV; //indice UV
  
  //Enviamos o json sensorData para o Firebase no path /History
  Firebase.push("/CACADOR", sensorData);
  //Limpamos os buffers dos jsons
  jsonBufferTimestamp.clear();
  jsonBufferSensor.clear();
}
