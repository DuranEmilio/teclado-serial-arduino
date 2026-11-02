# Teclado → Serial → Arduino

Cada tecla presionada en la computadora se envía por puerto serial a un Arduino, que muestra su valor
ASCII en 3 displays de 16 segmentos (`A` → `65`).

| Archivo | Qué hace |
|---|---|
| `teclado_serial.py` | Corre en la PC: lee el teclado y envía cada tecla |
| `firmware/firmware.ino` | Corre en el Arduino: recibe el byte y lo muestra en los displays |

## 1. Armar el circuito

**Materiales:** Arduino Uno o Nano, 3 displays de 16 segmentos (los 3 de cátodo común o los 3 de ánodo
común) y 10 resistencias de 1 kΩ.

Para formar números solo se usan los 10 segmentos del contorno y de la barra central:

```
  ── A1 ── ── A2 ──
 |                 |
 F                 B
 |                 |
  ── G1 ── ── G2 ──
 |                 |
 E                 C
 |                 |
  ── D1 ── ── D2 ──
```

Las diagonales y las verticales del centro no se conectan. Cada fabricante nombra los segmentos a su
manera, así que guíate por la posición del dibujo y busca en la hoja de datos de tu display qué pin
es cada segmento.

### Diagrama de conexiones

```
  ARDUINO             DISPLAY 1       DISPLAY 2       DISPLAY 3
                      (centenas)      (decenas)       (unidades)

  Pin 2  ──[1 kΩ]──────── A1 ──────────── A1 ──────────── A1
  Pin 3  ──[1 kΩ]──────── A2 ──────────── A2 ──────────── A2
  Pin 4  ──[1 kΩ]──────── B  ──────────── B  ──────────── B
  Pin 5  ──[1 kΩ]──────── C  ──────────── C  ──────────── C
  Pin 6  ──[1 kΩ]──────── D1 ──────────── D1 ──────────── D1
  Pin 7  ──[1 kΩ]──────── D2 ──────────── D2 ──────────── D2
  Pin 8  ──[1 kΩ]──────── E  ──────────── E  ──────────── E
  Pin 9  ──[1 kΩ]──────── F  ──────────── F  ──────────── F
  Pin 10 ──[1 kΩ]──────── G1 ──────────── G1 ──────────── G1
  Pin 11 ──[1 kΩ]──────── G2 ──────────── G2 ──────────── G2

  Pin 12 ──────────────── COM
  Pin 13 ──────────────────────────────── COM
  Pin A0 ──────────────────────────────────────────────── COM
```

Cada fila es un solo cable. Las filas de segmentos pasan por su resistencia y llegan al mismo segmento
de los 3 displays. Cada fila COM llega solo al pin común de su display (cátodo o ánodo, según tu
modelo). Las diagonales y las verticales del centro quedan sin conectar.

**Segmentos:** cada pin va, a través de una resistencia de 1 kΩ, al mismo segmento de los 3 displays
(A1 del primero con A1 del segundo y del tercero, y así con cada uno). Son 10 resistencias en total y
no 30, porque solo hay un display encendido a la vez.

| Segmento | A1 | A2 | B | C | D1 | D2 | E | F | G1 | G2 |
|---|---|---|---|---|---|---|---|---|---|---|
| Pin Arduino | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |

**Comunes:** van directo al Arduino, sin resistencia.

| Común del display | Pin Arduino |
|---|---|
| Centenas (izquierda) | 12 |
| Decenas (centro) | 13 |
| Unidades (derecha) | A0 |

Con resistencias de 1 kΩ, cada común conduce como máximo unos 30 mA, dentro del límite de 40 mA de un
pin del Arduino. Si los displays se ven muy tenues, no bajes las resistencias sin poner antes un
transistor en cada común.

## 2. Cargar el firmware

1. Abre `firmware/firmware.ino` en el IDE de Arduino.
2. Si tus displays son de ánodo común, cambia `ANODO_COMUN` a `true`.
3. Selecciona la placa y el puerto en *Herramientas* y presiona **Subir**.

Si el script de Python está corriendo, ciérralo antes de subir: mientras tenga el puerto abierto, la
carga falla.

| Ajuste en `firmware.ino` | Cuándo cambiarlo |
|---|---|
| `ANODO_COMUN` | Si tus displays son de ánodo común |
| `SEGMENTOS`, `DIGITOS` | Si conectaste los displays a otros pines |
| `delay(4)` | Si los displays parpadean: bájalo |

**Para probar sin Python:** abre el Monitor Serie a 9600 baudios y elige *Sin ajuste de línea*
(*No line ending*). Escribe `A` y envía: el display debe mostrar `65`. Con *Nueva línea* se manda
también un salto de línea después de la letra, y el display termina mostrando `10`.

## 3. Correr el programa en la PC

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

## Reinicio del Arduino

El Arduino se reinicia cuando el script abre el puerto, y por eso el script espera 2 segundos antes de
leer teclas. Si se pierden las primeras teclas, aumenta el `time.sleep(2)` en `teclado_serial.py`.
