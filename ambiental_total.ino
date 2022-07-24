#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT

#define FIREBASE_HOST "https://total-1b256.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 60000 //Intervalo de 1 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "AdylNet_Andressa_EXT"
#define PASSWORD "34557419"

//Objeto que realiza a leitura da umidade e temperatura
SimpleDHT11 dht;
//Variáveis do sensor DHT
float temperature, humidity;

//Variáveis do Sensor Capacitivo
#define soil 36
int umidadeSolo;
float readSoil(void);

//Variáveis de luminosidade
#define incidente 32
#define refletido 33
float readIncidente(void);
float readReflexivo(void);
int luxIncidente, luxRefletida;

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
   umidadeSolo = readSoil();  
   luxIncidente = readIncidente();
   luxRefletida = readReflexivo();
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
float readSoil(void){
  float umidade; 
  int leitura;
  leitura = analogRead(soil);  
  umidade = 100 * ((4095-(float)leitura) / 4095);  
  return umidade;    
}

float readIncidente(void){
  float luminosidade;
  int leitura;
  leitura = analogRead(incidente);
  luminosidade = 100 * ((4095-(float)leitura) / 4095);
  return luminosidade;  
}

float readReflexivo(void){
  float luminosidade;
  int leitura;
  leitura = analogRead(refletido);
  luminosidade = 100 * ((4095-(float)leitura) / 4095);
  return luminosidade;
}

//Envia a temperatura e umidade para o Firebase
void sendToFirebase() {
  //Buffer para a criação dos jsons
  StaticJsonBuffer<150> jsonBufferSensor;   
  //Criamos o objeto json que será enviado
  JsonObject& sensorData = jsonBufferSensor.createObject();   
  
  sensorData["T"] = temperature; //Inserimos os atributo de temperatura
  sensorData["H"] = humidity; //Inserimos os atributo de umidade  
  sensorData["Solo"] = umidadeSolo; //Inserimos os atributo Umidade do solo
  sensorData["Incidente"] = luxIncidente; //Inserimos os atributo de umidade  
  sensorData["Refletida"] = luxRefletida; //Inserimos os atributo Umidade do solo
  
  //Enviamos o json sensorData para o Firebase no path /History  
  Firebase.push("/TIAGO", sensorData);   
  
  jsonBufferSensor.clear();
 
}
