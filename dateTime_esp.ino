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
//Marca quando foi feita a última leitura
uint32_t lastTimeRead = 1601900057;

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
  Serial.println("Tentando Conectar...");

  //Enquanto não estiver conectado à rede WiFi
  while (WiFi.status() != WL_CONNECTED) {  
     delay(500);
     Serial.print(".");   
     WiFi.reconnect();  //forma 2 de reconectar
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

//Envia a temperatura e umidade para o Firebase
void sendToFirebase() {
  //Buffer para a criação dos jsons
  StaticJsonBuffer<150> jsonBufferSensor; 
  StaticJsonBuffer<50> jsonBufferTimestamp;
  //Criamos o objeto json que será enviado
  JsonObject& sensorData = jsonBufferSensor.createObject(); 
  sensorData["T"] = temperature; //Inserimos os atributo de temperatura
  sensorData["H"] = humidity; //Inserimos os atributo de umidade
  //Criamos o objeto json referente ao timestamp (Json auxiliar)
  JsonObject& timestamp = jsonBufferTimestamp.createObject();
  //Marcamos como um valor que o server do Firebase irá preencher com o timestamp
  timestamp[".sv"] = "timestamp"; 
  //Inserimos o json referente ao timestamp no json que será enviado
  sensorData["timestamp"] = timestamp; 
  //Enviamos o json sensorData para o Firebase no path /History
  Firebase.push("/History", sensorData);
  //Limpamos os buffers dos jsons
  jsonBufferTimestamp.clear();
  jsonBufferSensor.clear();
}
