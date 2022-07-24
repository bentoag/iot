int pino_sensor = 2;
int estado_sensor;
int tempo_sensor;

// Definições do módulo
#define DESLIGADO 0 
#define LIGANDO 1
#define LIGADO 2
#define DESLIGANDO 3
#define TEMPO_DEBOUNCE 300   // tempo de debouce

void setup() 
{ 
  pinMode(pino_sensor, INPUT); 
  tempo_sensor = millis(); // marca tempo inicial
  estado_sensor =  DESLIGADO;
  Serial.begin(9600);
} 

void loop() 
{ 

  int sensor =  digitalRead(pino_sensor); 
  sensor_controlar(sensor);   

} 


void sensor_controlar(unsigned char entrada){
  switch(estado_sensor){
  case DESLIGADO:
    {              // estado DESLIGADO
      if (entrada == 1){          // se entrada = 1
        estado_sensor = LIGANDO;  // vai para estado ligando
        tempo_sensor = millis();     // marca tempo
        Serial.println("LIGANDO");
      }
      break;                      // fim deste estado
    }
  case LIGANDO:
    {                // estado LIGANDO
      if (entrada == 0){          // se entrada voltou pra zero
        estado_sensor = DESLIGADO; // sensor volta para desligado(pulso falso)
        Serial.println("DESLIGADO-PULSO FALSO");
      }
      else if (millis() > tempo_sensor+TEMPO_DEBOUNCE)// se entrada permaneceu 30x100ms em 1
      {   
        estado_sensor = LIGADO;    // valida sensor colocando o sensor em LIGADO
        Serial.println("LIGADO");
      }
      break;
    }
  case LIGADO:
    {                  //estado LIGADO
      if (entrada == 0){          // se entrada voltou pra zero 
        estado_sensor = DESLIGANDO; // estado vai para desligando
        tempo_sensor = millis();     // marca tempo
        Serial.println("DESLIGANDO");
      }		    
      break;
    }	      
  case DESLIGANDO:
    {             // estado DESLIGANDO
      if (entrada == 1){          // se entrada voltou pra 1 
        estado_sensor = LIGADO;  // estado volta para ligado(pulso falso)
        Serial.println("LIGADO-PULSO FALSO");        
      }
      else if (millis() > tempo_sensor+TEMPO_DEBOUNCE)// se entrada permaneceu 30x100ms em 1
      {   
        estado_sensor = DESLIGADO; // valida sensor colocando em estado DESLIGADO
        Serial.println("DESLIGADO");        
      }
      break;
    }
  }
}



