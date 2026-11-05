import sys
import time
import serial

PUERTO = sys.argv[1] if len(sys.argv) > 1 else "COM5"
BAUDIOS = 9600

try:
    import msvcrt
    def leer_tecla():
        tecla = msvcrt.getwch()
        if tecla in ("\x00", "\xe0"):
            msvcrt.getwch()
            return ""
        return tecla
except ImportError:
    import termios
    import tty
    def leer_tecla():
        fd = sys.stdin.fileno()
        anterior = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            return sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, anterior)

with serial.Serial(PUERTO, BAUDIOS, timeout=1) as arduino:
    time.sleep(2)
    print(f"Conectado a {PUERTO}. Presiona teclas (ESC o Ctrl+C para salir).")
    while True:
        tecla = leer_tecla()
        if tecla in ("\x1b", "\x03"):
            break
        if not tecla or not tecla.isascii():
            continue
        arduino.write(tecla.encode("ascii"))
        print(f"Enviado: {tecla!r} (ASCII {ord(tecla)})")