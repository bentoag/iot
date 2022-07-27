#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT

#define FIREBASE_HOST "https://cacador-ifsc.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 300000 //Intervalo de 5 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "VETORIAL_356"
#define PASSWORD "50599882"

//Objeto que realiza a leitura da umidade e temperatura
SimpleDHT11 dht;
//Variáveis do sensor DHT
float temperature, humidity;

//Variáveis do Higrometro
#define higrometro 36
int umidadeSolo;
float readHigrometro(void);

//variáveis UV Sensor
#define uvSensor 32
int indexUV;
int readUVSensor();

//Marca quando foi feita a última leitura
uint32_t lastTimeRead = 0;

void setup() {
  Serial.begin(115200); 
  //Inicializa a conexão com a rede WiFi
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

//Conexão com a rede WiFi
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

//HIGROMETRO
float readHigrometro(void){
  float umidade; 
  int leitura;
  leitura = analogRead(higrometro);  
  umidade = 100 * ((4095-(float)leitura) / 4095);  
  return umidade;    
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

//Envia a temperatura e umidade para o Firebase
void sendToFirebase() {
  //Buffer para a criação dos jsons
  StaticJsonBuffer<150> jsonBufferSensor;   
  //Criamos o objeto json que será enviado
  JsonObject& sensorData = jsonBufferSensor.createObject();   
  
  sensorData["T"] = temperature; //Inserimos os atributo de temperatura
  sensorData["H"] = humidity; //Inserimos os atributo de umidade  
  sensorData["Index UV"] = indexUV; //indice UV
  
  //Enviamos o json sensorData para o Firebase no path /History  
  Firebase.push("CACADOR:", sensorData);   
  jsonBufferSensor.clear();
 
}
