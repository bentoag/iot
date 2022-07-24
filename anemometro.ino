class anemometro;

      // --- Constantes ---
        const float pi = 3.14159265;     //NÃºmero de pi
        int period = 5000;               //Tempo de medida(miliseconds)
        public int delaytime = 1000;            //Invervalo entre as amostras (miliseconds)
        int radius = 147;                //Raio do anemometro(mm)
        
      // --- Variaveis Globais ---
        unsigned int counter = 0;        //Contador para o sensor
        unsigned int RPM = 0;            //RotaÃ§Ãµes por minuto
        float windspeed = 0;             //Velocidade do vento (m/s)

//Funcao para medir velocidade do vento
void windvelocity() {
  windspeed = 0;
  counter = 0;
  attachInterrupt(0, addcount, RISING);
  unsigned long millis();
  long startTime = millis();
  while (millis() < startTime + period) {}
}

//FunÃ§Ã£o para calcular o RPM
void RPMcalc() {
  RPM = ((counter) * 60) / (period / 1000); // Calculate revolutions per minute (RPM)
}

//Velocidade do vento em m/s
void WindSpeed() {
  windspeed = ((4 * pi * radius * RPM) / 60) / 1000; //Calcula a velocidade do vento em m/s
} //end WindSpeed

//Incrementa contador
void addcount() {
  counter++;
}
