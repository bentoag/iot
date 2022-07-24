#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT
#include <Wire.h>


#define FIREBASE_HOST "https://uvindexfln-default-rtdb.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 60000 //Intervalo de 5 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "Allan"
#define PASSWORD "bls_1705"

//Objeto que realiza a leitura da umidade e temperatura
SimpleDHT11 dht;
//Variáveis do sensor DHT
float temperature, humidity;

//Sensor UV
int indiceUV = 0;

//valor referente a um timestemp prox a leitura(07/10/2020 18:30:00)
uint32_t lastTimeRead = 1610288457;

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

  if(WiFi.status() == WL_CONNECTED){
       //Se passou o intervalo desde a última leitura 
        if (now - lastTimeRead > INTERVAL) {
         readDht();
         readUV();    
         sendToFirebase();     
         lastTimeRead = now;
      }     
  }else{
    WiFi.reconnect();
  }
     
}

/*############ CONEXAO COM A REDE WIFI ############ */
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
  2. HIGROMETRO CAPACITIVO - retorna um valor inteiro referente a umidade do solo
  3. Leituras do LDR: Radiacao incidente e refletida
  4. Fluxo de calor - retorna um valor float com base nas duas temperaturas
*/
void readDht() {
  float t, h;
  //Coloca o valor lido da temperatura em t e da umidade em h
  int status = dht.read2(DHTPIN, &t, &h, NULL);
  //Se a leitura foi bem sucedida
  if (status == SimpleDHTErrSuccess) {
    //Os valores foram lidos corretamente, então é seguro colocar nas variáveis
    temperature = (t-2);
    humidity = (h+5);
  }
}

void readUV(){

  //Leitura dos dados do sensor
  int leitura_porta = (analogRead(A0));
  Serial.println(leitura_porta);
  //De acordo com a leitura define o indice UV corrrspondente
  if (leitura_porta <= 10) {
    indiceUV = 0;
  } else if (leitura_porta > 10 && leitura_porta <= 46) {
    indiceUV = 1;
  } else if (leitura_porta > 46 && leitura_porta <= 65) {
    indiceUV = 2;
  } else if (leitura_porta > 65 && leitura_porta <= 83) {
    indiceUV = 3;
  } else if (leitura_porta > 83 && leitura_porta <= 103) {
    indiceUV = 4;
  } else if (leitura_porta > 103 && leitura_porta <= 124) {
    indiceUV = 5;
  } else if (leitura_porta > 124 && leitura_porta <= 142) {
    indiceUV = 6;
  } else if (leitura_porta > 142 && leitura_porta <= 162) {
    indiceUV = 7;
  } else if (leitura_porta > 162 && leitura_porta <= 180) {
    indiceUV = 8;
  } else if (leitura_porta > 180 && leitura_porta <= 200) {
    indiceUV = 9;
  } else if (leitura_porta > 200 && leitura_porta <= 221) {
    indiceUV = 10;
  } else {
    indiceUV = 11;
  }
}
/*############ ENVIA OS VALORES PARA O BANCO DE DADOS - FIREBASE ############
  1. Data e Hora (timestamp)  
  2. DHT11 - Temperatura e Umidade
  3. LDR - Incidente e Refletida
  4. Fluxo de calor 
*/

void sendToFirebase() {
  //Buffer para a criação dos jsons
  StaticJsonBuffer<150> jsonBufferSensor;  
  StaticJsonBuffer<50> jsonBufferTimestamp; 
  //Criamos o objeto json que será enviado  
  JsonObject& timestamp = jsonBufferTimestamp.createObject();
  //Marcamos como um valor que o server do Firebase irá preencher com o timestamp
  timestamp[".sv"] = "timestamp"; 
  JsonObject& sensorData = jsonBufferSensor.createObject();   

  sensorData["MS_J"] = timestamp; //Inserimos o json referente ao timestamp no json que será enviado
  sensorData["T"] = temperature; //Inserimos os atributo de temperatura
  sensorData["H"] = humidity; //Inserimos os atributo de umidade  
  sensorData["Indice UV"] = indiceUV; //Inserimos os atributo Umidade do solo
 
  //Enviamos o json sensorData para o Firebase no path /ESTACAO_01
  Firebase.push("/ESTACAO01", sensorData);     
  
  jsonBufferTimestamp.clear();
  jsonBufferSensor.clear(); 
}
