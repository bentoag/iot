#include <WiFi.h>
#define INTERVAL 5000 //Intervalo de 5 min entre cada leitura do sensor

//SSID e senha da rede WiFi onde o esp32 irá se conectar
#define SSID "VETORIAL_356"
#define PASSWORD "50599882"

//Variáveis do Higrometro
#define higrometro 36

void setup() {
  Serial.begin(9600); 
  //Inicializa a conexão com a rede WiFi
  setupWiFi(); 
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

void loop() {    
   int sensorValue = analogRead(higrometro);
  Serial.println(sensorValue);  
  delay(INTERVAL);
      
}
