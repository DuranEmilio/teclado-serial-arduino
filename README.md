# Teclado → Serial → Arduino

Cada tecla presionada en la computadora se envía por puerto serial a un Arduino, que muestra en una
pantalla LCD 16x2 la tecla y su valor ASCII en decimal, hexadecimal y binario:

```
┌────────────────┐
│T:A Dec:65      │
│H:41 B:01000001 │
└────────────────┘
```

| Archivo | Qué hace |
|---|---|
| `teclado_serial.py` | Corre en la PC: lee el teclado y envía cada tecla |
| `firmware/firmware.ino` | Corre en el Arduino: recibe el byte y lo muestra en la LCD |

## 1. Armar el circuito

**Materiales:** Arduino Uno o Nano y una LCD 16x2 con módulo I2C soldado atrás (el que tiene 4 pines).

### Diagrama de conexiones

```
  ARDUINO          MÓDULO I2C (atrás de la LCD)

  GND  ─────────── GND
  5V   ─────────── VCC
  A4   ─────────── SDA
  A5   ─────────── SCL
```

## 2. Cargar el firmware

1. En el IDE de Arduino, abre *Herramientas → Administrar bibliotecas*, busca **LiquidCrystal I2C** e
   instala la de **Frank de Brabander**.
2. Abre `firmware/firmware.ino`.
3. Selecciona la placa y el puerto en *Herramientas* y presiona **Subir**.

Si el script de Python está corriendo, ciérralo antes de subir: mientras tenga el puerto abierto, la
carga falla.

Al encender, la pantalla muestra `Listo: Presiona una tecla...`. Si no se ve así:

| Problema | Solución |
|---|---|
| Hay luz pero no hay texto, o solo se ven cuadros | Gira el potenciómetro azul del módulo I2C: es el contraste |
| No aparece nada | Cambia `0x27` por `0x3F` en `firmware.ino`: es la otra dirección común del módulo |

**Para probar sin Python:** abre el Monitor Serie a 9600 baudios y elige *Sin ajuste de línea*
(*No line ending*). Escribe `A` y envía: la pantalla debe mostrar `T:A Dec:65`. Con *Nueva línea*
se manda también un salto de línea después de la letra, y la pantalla termina mostrando el del salto
de línea (`Dec:10`).

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

Si no pasas el puerto, el script usa `COM5`. Cada tecla se envía en cuanto se presiona, sin esperar
Enter. Para salir: **ESC** o **Ctrl+C**.

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

Enter, Tab, Retroceso y Ctrl + letra no tienen un carácter que se pueda dibujar: después de `T:` la
LCD muestra un símbolo cualquiera, pero `Dec`, `H` y `B` salen bien. En macOS/Linux las flechas
empiezan con ESC, así que presionar una cierra el programa.

## Reinicio del Arduino

El Arduino se reinicia cuando el script abre el puerto, y por eso el script espera 2 segundos antes de
leer teclas. Si se pierden las primeras teclas, aumenta el `time.sleep(2)` en `teclado_serial.py`.
