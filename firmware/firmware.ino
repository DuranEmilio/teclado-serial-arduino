// Recibe por serial 1 byte por tecla y muestra su valor ASCII (0-127) en 3 displays de 16 segmentos.
// Solo se usan los 10 segmentos que forman números; diagonales y verticales centrales quedan sin conectar.

const bool ANODO_COMUN = false;  // true si tus displays son de ánodo común

// Pines de los segmentos, compartidos por los 3 displays
//                         A1 A2 B  C  D1 D2 E  F  G1  G2
const int SEGMENTOS[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
// Pin del común de cada display: centenas, decenas, unidades
const int DIGITOS[3] = {12, 13, A0};

// 1 = segmento encendido, en el mismo orden que SEGMENTOS
const byte NUMEROS[10][10] = {
  // A1 A2 B  C  D1 D2 E  F  G1 G2
  {1, 1, 1, 1, 1, 1, 1, 1, 0, 0},  // 0
  {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},  // 1
  {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},  // 2
  {1, 1, 1, 1, 1, 1, 0, 0, 1, 1},  // 3
  {0, 0, 1, 1, 0, 0, 0, 1, 1, 1},  // 4
  {1, 1, 0, 1, 1, 1, 0, 1, 1, 1},  // 5
  {1, 1, 0, 1, 1, 1, 1, 1, 1, 1},  // 6
  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  // 8
  {1, 1, 1, 1, 1, 1, 0, 1, 1, 1},  // 9
};

const int SEG_ENCENDIDO = ANODO_COMUN ? LOW : HIGH;
const int DIG_ENCENDIDO = ANODO_COMUN ? HIGH : LOW;

int valor = -1;  // -1 = todavía no llega ninguna tecla

void setup() {
  Serial.begin(9600);  // misma velocidad que teclado_serial.py
  for (int pin : SEGMENTOS) pinMode(pin, OUTPUT);
  for (int pin : DIGITOS) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, !DIG_ENCENDIDO);
  }
}

void loop() {
  if (Serial.available()) valor = Serial.read();  // el byte ya es el valor ASCII: 'A' -> 65
  if (valor < 0) return;

  int cifras[3] = {valor / 100, valor / 10 % 10, valor % 10};
  int primera = valor >= 100 ? 0 : valor >= 10 ? 1 : 2;  // sin ceros a la izquierda

  // Multiplexado: se enciende un display a la vez, tan rápido que se ven los 3 juntos
  for (int d = 0; d < 3; d++) {
    for (int s = 0; s < 10; s++) digitalWrite(SEGMENTOS[s], NUMEROS[cifras[d]][s] ? SEG_ENCENDIDO : !SEG_ENCENDIDO);
    if (d >= primera) digitalWrite(DIGITOS[d], DIG_ENCENDIDO);
    delay(4);  // milisegundos por display; si se nota parpadeo, bajarlo
    digitalWrite(DIGITOS[d], !DIG_ENCENDIDO);
  }
}
