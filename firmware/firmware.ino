#include <LiquidCrystal.h>


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C común: 0x27 o 0x3F para pantallas 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); 

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
  if (Serial.available() > 0) {
    char tecla = Serial.read();
    
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
}