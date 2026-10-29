# Teclado → Serial → Arduino

Programa en Python que lee cada tecla presionada en la computadora y la envía por puerto serial.
Del otro lado, un Arduino recibe el byte y muestra su valor ASCII en un display de 16 segmentos
(el firmware no está en este repo).

## Uso

```bash
pip install pyserial
python teclado_serial.py <PUERTO>
```

| Sistema | Ejemplo de puerto |
|---|---|
| Windows | `COM3` (ver en el Administrador de dispositivos) |
| Linux | `/dev/ttyACM0` o `/dev/ttyUSB0` |
| macOS | `/dev/tty.usbmodem1101` (`ls /dev/tty.usb*`) |

Cada tecla se envía en cuanto se presiona, sin esperar Enter. Para salir: **ESC** o **Ctrl+C**.

> Cierra el Monitor Serie del IDE de Arduino antes de correr el script: si lo dejas abierto, el puerto queda ocupado.

## Lo que recibe el firmware

| Parámetro | Valor |
|---|---|
| Velocidad | **9600 baudios** |
| Formato | 8N1 (el default de `Serial.begin`) |
| Por cada tecla | **1 byte** con su código ASCII (0–127) |
| Separador | Ninguno: no se agrega `\n` ni `\r` al final |

El byte que llega **ya es el valor ASCII**, no hay que convertirlo: `'A'` llega como `65`, `'0'` como `48`.

### Teclas especiales

| Tecla | Byte que llega |
|---|---|
| Letras, números, símbolos | 32–126 |
| Espacio | 32 |
| Enter | 13 (`\r`) |
| Tab | 9 |
| Retroceso | 127 en macOS/Linux, 8 en Windows |
| Ctrl + letra | 1–26 (Ctrl+A = 1) |
| ESC, Ctrl+C | No se envían: cierran el programa |
| ñ, acentos, flechas, F1–F12 | No se envían |

En macOS/Linux las flechas empiezan con ESC, así que presionar una cierra el programa.

## Ejemplo mínimo de firmware

```cpp
void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    int ascii = Serial.read();  // 'A' -> 65
    // mostrarEnDisplay(ascii);
  }
}
```

## Reinicio del Arduino

El Arduino se reinicia cuando el script abre el puerto, y por eso el script espera 2 segundos antes de
leer teclas. Si se pierden las primeras teclas, aumenta el `time.sleep(2)` en `teclado_serial.py`.
