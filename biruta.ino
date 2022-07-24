// Definicoes Biruta
int dirpin = A1;
String direcao;
float leiturabrt = 0;
int Winddir = 0;

//Funcoes para indicar a direÃ§Ã£o do vento
void WindDir() {
  leiturabrt = analogRead(dirpin);
  if (leiturabrt >= 744 && leiturabrt <= 805) {
    Winddir = 360;
    direcao = "NORTE";
  } else if (leiturabrt >= 806 && leiturabrt <= 885) {
    Winddir = 337, 5;
    direcao = "Norte-Noroeste";
  } else if (leiturabrt >= 433 && leiturabrt <= 529) {
    Winddir = 315;
    direcao = "Noroeste";
  } else if (leiturabrt >= 346 && leiturabrt <= 432) {
    Winddir = 292, 5;
    direcao = "Oeste-Noroeste";
  } else if (leiturabrt >= 886 && leiturabrt <= 961) {
    Winddir = 270;
    direcao = "Oeste";
  } else if (leiturabrt >= 530 && leiturabrt <= 614) {
    Winddir = 247, 5;
    direcao = "Oeste-Sudoeste";
  } else if (leiturabrt >= 615 && leiturabrt <= 665) {
    Winddir = 225;
    direcao = "Sudoeste";
  } else if (leiturabrt >= 214 && leiturabrt <= 264) {
    Winddir = 202, 5;
    direcao = "Sul-Sudoeste";
  } else if (leiturabrt >= 265 && leiturabrt <= 345) {
    Winddir = 180;
    direcao = "SUL";
  } else if (leiturabrt >= 110 && leiturabrt <= 155) {
    Winddir = 157, 5;
    direcao = "Sul-Sudeste";
  } else if (leiturabrt >= 156 && leiturabrt <= 213) {
    Winddir = 135;
    direcao = "Sudeste";
  } else if (leiturabrt >= 0 && leiturabrt <= 74) {
    Winddir = 112, 5;
    direcao = "Leste-Sudeste";
  } else if (leiturabrt >= 88 && leiturabrt <= 109) {
    Winddir = 90;
    direcao = "LESTE";
  } else if (leiturabrt >= 75 && leiturabrt <= 87) {
    Winddir = 67, 5;
    direcao = "Leste-Nordeste";
  } else if (leiturabrt >= 433 && leiturabrt <= 529) {
    Winddir = 45;
    direcao = "Nordeste";
  } else if (leiturabrt >= 346 && leiturabrt <= 432) {
    Winddir = 22, 5;
    direcao = "Norte-Nordeste";
  }
  else {
    Winddir = 000;
  }
}
