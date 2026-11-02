// Recibe por serial 1 byte por tecla y muestra en una LCD 16x2 (I2C) la tecla y su valor ASCII
// en decimal, hexadecimal y binario:
//   'A'  ASCII:65
//   0x41  0b01000001
// Requiere la librería "LiquidCrystal I2C" de Frank de Brabander (Gestor de librerías del IDE).

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // dirección I2C del módulo; si la pantalla no muestra nada, prueba 0x3F

void setup() {
  Serial.begin(9600);  // misma velocidad que teclado_serial.py
  lcd.init();
  lcd.backlight();
  lcd.print("Esperando tecla");
}

void loop() {
  if (!Serial.available()) return;
  byte ascii = Serial.read();  // el byte ya es el valor ASCII: 'A' -> 65

  char binario[9];
  for (int i = 0; i < 8; i++) binario[i] = bitRead(ascii, 7 - i) ? '1' : '0';
  binario[8] = '\0';

  // Enter, Tab, Retroceso, etc. no tienen dibujo: se muestra un bloque lleno (0xFF en la LCD)
  char tecla = ascii >= 32 && ascii < 127 ? ascii : 0xFF;

  char linea[17];
  lcd.clear();
  snprintf(linea, sizeof linea, "'%c'  ASCII:%d", tecla, ascii);
  lcd.print(linea);
  snprintf(linea, sizeof linea, "0x%02X  0b%s", ascii, binario);
  lcd.setCursor(0, 1);
  lcd.print(linea);
}
