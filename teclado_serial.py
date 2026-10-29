"""Lee cada tecla presionada y la envía por puerto serial al Arduino.

Uso:  python teclado_serial.py COM3            (Windows)
      python teclado_serial.py /dev/ttyACM0    (Linux)
      python teclado_serial.py /dev/tty.usbmodem1101   (macOS)
Requiere: pip install pyserial
"""
import sys
import time

import serial

PUERTO = sys.argv[1] if len(sys.argv) > 1 else "COM3"
BAUDIOS = 9600  # debe coincidir con Serial.begin() del firmware

try:
    import msvcrt  # Windows

    def leer_tecla():
        tecla = msvcrt.getwch()
        if tecla in ("\x00", "\xe0"):  # flechas y F1-F12 llegan como 2 códigos
            msvcrt.getwch()
            return ""
        return tecla
except ImportError:  # macOS / Linux
    import termios
    import tty

    def leer_tecla():
        fd = sys.stdin.fileno()
        anterior = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)  # entrega la tecla sin esperar Enter
            return sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, anterior)


with serial.Serial(PUERTO, BAUDIOS, timeout=1) as arduino:
    time.sleep(2)  # el Arduino se reinicia al abrir el puerto; ajustar si se pierden las primeras teclas
    print(f"Conectado a {PUERTO}. Presiona teclas (ESC o Ctrl+C para salir).")
    while True:
        tecla = leer_tecla()
        if tecla in ("\x1b", "\x03"):  # ESC, Ctrl+C
            break
        if not tecla or not tecla.isascii():
            print("Tecla especial o no ASCII, se ignora")
            continue
        arduino.write(tecla.encode("ascii"))
        print(f"Enviado: {tecla!r} (ASCII {ord(tecla)})")
