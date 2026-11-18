Import("env")

import time
import serial


def reset_nano(source, target, env):
    port = env.subst("$UPLOAD_PORT")
    with serial.Serial(port, 115200, timeout=0.2) as connection:
        connection.dtr = False
        connection.rts = False
        time.sleep(0.2)
        connection.dtr = True
        time.sleep(0.1)
        connection.dtr = False
        connection.rts = True
        time.sleep(0.1)
        connection.rts = False
    time.sleep(0.3)


env.AddPreAction("upload", reset_nano)
