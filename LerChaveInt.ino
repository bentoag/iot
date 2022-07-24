int pino_led1 = 13; 
int pino_led2 = 12; 
int pino_chave = 2; 
volatile int estado = LOW; 

void setup() 
{ 
   pinMode(pino_led1, OUTPUT); 
   pinMode(pino_led2, OUTPUT); 
   digitalWrite(pino_led1,LOW);
   pinMode(pino_chave, INPUT); 
   digitalWrite(pino_chave,HIGH); // Ativa pull UP chave
   attachInterrupt(0, pisca, CHANGE);  // Ativa interrupção na chave
} 

void loop() 
{ 
   // pisca o led a cada 3s
   digitalWrite(pino_led2, HIGH);
   delay(3000); 
   digitalWrite(pino_led2, LOW);
   delay(3000); 
} 

// Interrupcao gerada quando a chave e pressionada
void pisca() 
{ 
   if (digitalRead(pino_chave)==LOW)
      digitalWrite(pino_led1, HIGH); 
   else digitalWrite(pino_led1, LOW); 
}
