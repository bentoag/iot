#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <Arduino_JSON.h> //Lib para a manipulação de Json
#include <SimpleDHT.h> //Lib DHT

#define FIREBASE_HOST "https://dthsolo-default-rtdb.firebaseio.com/" // URL da base de dados fornecido pelo Firebase para a conexão http
#define FIREBASE_AUTH "" // Autenticação (no caso não estamos usando)

#define DHTPIN 4 //Pino onde o DHT está ligado
#define INTERVAL 60000 //Intervalo de 15 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "Allan"
#define PASSWORD "bls_1705"

//Objeto que realiza a leitura da umidade e temperatura
SimpleDHT11 dht;
//Variáveis do sensor DHT
float temperature, humidity;

//Variáveis do Sensor Capacitivo
#define soil 36
int umidadeSolo;
float umiSoloAjust;
float readSoil(void);


//valor referente a um timestemp prox a leitura(11/01/2021 09:56:00)
uint32_t lastTimeRead = 1610369776;

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
         umidadeSolo = readSoil();         
         readDht();    
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

//HIGROMETRO
float readSoil(void){
  float umidade; 
  int leitura;
  leitura = analogRead(soil);  
  umidade = 100 * ((4095-(float)leitura) / 4095); 
  umiSoloAjust = ((umidade*0.897435947460197)); 
  return umiSoloAjust;    
}


/*############ ENVIA OS VALORES PARA O BANCO DE DADOS - FIREBASE ############
  1. Data e Hora (timestamp)  
  2. DHT11 - Temperatura e Umidade 
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
  sensorData["Solo"] = umidadeSolo; //Inserimos os atributo Umidade do solo  
    
  //Enviamos o json sensorData para o Firebase no path /ESTACAO_01
  Firebase.push("/ESTACAO01", sensorData);     
  
  jsonBufferTimestamp.clear();
  jsonBufferSensor.clear(); 
}
