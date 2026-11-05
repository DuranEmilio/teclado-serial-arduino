#include <LiquidCrystal.h>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C común: 0x27 o 0x3F para pantallas 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tiempo mínimo (ms) que se muestra cada tecla antes de pasar a la siguiente
const unsigned long TIEMPO_POR_TECLA = 1000;

// FIFO circular: guarda las teclas que llegan mientras otra se está mostrando
const int TAMANO_FIFO = 64;
char fifo[TAMANO_FIFO];
int inicio = 0;    // posición de la tecla más vieja
int cantidad = 0;  // teclas en espera
unsigned long ultimaMostrada = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Listo: Presiona");
  lcd.setCursor(0, 1);
  lcd.print("una tecla...");
}

void loop() {
  // Polling: en cada vuelta se revisa el serial y lo que llegó se forma al final de la FIFO
  while (Serial.available() > 0) {
    char tecla = Serial.read();
    if (cantidad < TAMANO_FIFO) {
      fifo[(inicio + cantidad) % TAMANO_FIFO] = tecla;
      cantidad++;
    }  // si la FIFO está llena, la tecla se descarta
  }

  // Cuando la tecla en pantalla cumplió su tiempo, se saca la más vieja de la FIFO
  if (cantidad > 0 && millis() - ultimaMostrada >= TIEMPO_POR_TECLA) {
    char tecla = fifo[inicio];
    inicio = (inicio + 1) % TAMANO_FIFO;
    cantidad--;
    mostrar(tecla);
    ultimaMostrada = millis();
  }
}

void mostrar(char tecla) {
  lcd.clear();

  // Fila 0: Muestra la tecla y el valor decimal (ej: "T:9 Dec:57")
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(tecla);
  lcd.print(" Dec:");
  lcd.print((int)tecla);

  // Fila 1: Muestra Hex y Binario ajustado a 8 bits (ej: "H:39 B:00111001")
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(tecla, HEX);
  lcd.print(" B:");

  // Imprimimos el binario rellenando con ceros a la izquierda para completar 8 bits
  byte val = (byte)tecla;
  for (int i = 7; i >= 0; i--) {
    lcd.print((val >> i) & 1);
  }
}
