int pino_led1 = 13; 
int pino_led2 = 12; 

long tempo_led1=0;
long tempo_led2=0;
int estado_led1 = LOW; 
int estado_led2 = LOW; 

void setup() 
{ 
   pinMode(pino_led1, OUTPUT); 
   pinMode(pino_led2, OUTPUT); 
   tempo_led1 = millis(); // marca tempo inicial
   tempo_led2 = millis(); 
} 

void loop() 
{ 
   // verificar se passou 3s do tempo marcado
   if (millis() >= tempo_led1 + 3000){
      estado_led1 = !estado_led1; // inverte estado do led
      digitalWrite(pino_led1, estado_led1);     
      tempo_led1 = millis(); // marca novo tempo
   }
   if (millis() >= tempo_led2 + 500){
      estado_led2 = !estado_led2; // inverte estado do led
      digitalWrite(pino_led2, estado_led2);     
      tempo_led2 = millis(); // marca novo tempo
   }   
} 


