
// Programa : Esteira Canaril versao 1.0 13/jul/2020
// Autor : Bento Almeida Gonzaga  

#include <AccelStepper.h>

// Definições das Portas Digitais do Arduino

int RST = 8;              // Porta digital D08 - reset do A4988
int SLP = 9;              // Porta digital D09 - dormir (sleep) A4988
int ENA = 7;              // Porta digital D07 - ativa (enable) A4988
int MS1 = 4;              // Porta digital D04 - MS1 do A4988
int MS2 = 5;              // Porta digital D05 - MS2 do A4988
int MS3 = 6;              // Porta digital D06 - MS3 do A4988
int DIR = 3;              // Porta digital D03 - direção (direction) do A4988
int STP = 2;              // Porta digital D02 - passo(step) do A4988

int velocidade_motor = 500; 
int aceleracao_motor = 100;
int sentido_horario = 0;
int sentido_antihorario = 0;
int numero = 0; 

void setup() {

  Serial.begin(9600);
  pinMode(pino_enable, OUTPUT);
  DDRD = DDRD | B11111100;  // Configura Portas D02 até D07 como saída
  disa_A4988();             // Desativa o chip A4988
  DDRB = 0x0F;              // Configura Portas D08,D09,D10 e D11 como saída
  digitalWrite(SLP, HIGH);  // Desativa modo sleep do A4988
  rst_A4988();              // Reseta o chip A4988
  ena_A4988();              // Ativa o chip A4988

   // Configuracoes iniciais motor de passo
  motor1.setMaxSpeed(velocidade_motor);
  motor1.setSpeed(velocidade_motor);
  motor1.setAcceleration(aceleracao_motor);

  Serial.println("Digite 1, 2 ou 3 e clique em ENVIAR...");
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

//FUNCOES DE CONTROLE DO MOTOR

void rst_A4988()
{
  digitalWrite(RST, LOW);     // Realiza o reset do A4988
  delay (10);                 // Atraso de 10 milisegundos
  digitalWrite(RST, HIGH);    // Libera o reset do A4988
  delay (10);                 // Atraso de 10 milisegundos
}

void disa_A4988()
{
  digitalWrite(ENA, HIGH);    // Desativa o chip A4988
  delay (10);                 // Atraso de 10 milisegundos
}

void ena_A4988()
{
  digitalWrite(ENA, LOW);     // Ativa o chip A4988
  delay (10);                 // Atraso de 10 milisegundos
}

void HOR()                      // Configura o sentido de rotação do Motor
{
  Serial.println(" Sentido Horario ");
  digitalWrite(DIR, HIGH);      // Configura o sentido HORÁRIO
}

void AHR()                      // Configura o sentido de rotação do Motor
{
  Serial.println(" Sentido anti-Horario ");
  digitalWrite(DIR, LOW);       // Configura o sentido ANTI-HORÁRIO
}
