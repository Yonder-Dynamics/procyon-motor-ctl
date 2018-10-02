#!/usr/bin/env python3
ENABLE_SERIAL = True
if ENABLE_SERIAL:
    from serial import Serial
import time
import math
import json

import redis

#addr = "00:14:03:05:F1:F5"
MSG_RATE  = 10 #in Hz
MSG_PER = 1./MSG_RATE
MAX_TURNING_RADIUS = 10
TIMEOUT = 2
CLEAR_BUFFER = 2
BAUDRATE = [9600,57600,115200][2]
buffer_count = 0
# Always use serial ports like this because they don't change
serial_port = "/dev/serial/by-id/usb-Arduino_Srl_Arduino_Mega_55635303838351816162-if00"


MSG_TYPES = {
    "drive": 0,
    "motors": 0,
    "arm": 1,
    "drill": 2,
    "terr": 3,
    "manual": 4,
}

def openSerial():
    global s
    s = Serial(serial_port, BAUDRATE)
    time.sleep(1)

# Open serial. DO NOT REMOVE DELAY
print("Opening serial")
if ENABLE_SERIAL:
    print("Looking for serial port: %s ..." % serial_port)
    openSerial()
print("Started listening")

last_message_send = 0
is_stopped = False

# data: [int]
def makeSerialMsg(msg):
    serialMsg = '#%i#' % MSG_TYPES[msg.type] + ('{},' * len(msg.data) + '\n')
    serialMsg = serialMsg.format(*msg.data)
    print(serialMsg)
    return serialMsg

def kill_callback(msg):
    global is_stopped
    is_stopped = msg.data
    #s.write(makeSerialMsg([0]*6))

def callback(msg):
    global s, is_stopped, last_message_send, buffer_count
    # Kill switch
    if is_stopped:
        return
    if msg.type == "terr":
        #s.write("#3#")
        pass

    if time.time() - last_message_send < MSG_PER:
        return
    last_message_send = time.time()

    serialMsg = makeSerialMsg(msg)
    # Check for out of bounds ints
    for x in msg.data:
        if x > 255:
            s.write(makeSerialMsg([0] * 6))
            return
    if buffer_count > CLEAR_BUFFER:
        s.reset_input_buffer()
        s.reset_output_buffer()
    buffer_count += 1


    if ENABLE_SERIAL:
        out = s.read(s.inWaiting()).decode('ascii')
        print(out)
        # Reopen serial
        try:
            s.write(bytes(serialMsg, 'utf-8'))
        except:
            openSerial()
            print("Failed to write to serial")
        # Read data from serial as well

def init():
    global last_message_send
    # Check if still recieving messages
    #while not rospy.is_shutdown():
    #    if last_message_send > TIMEOUT:
    #        s.write(makeSerialMsg([0] * 6))
    #    rospy.spin_once()
    # s.write(makeSerialMsg([0] * 6))
    # s.close()
    client = redis.StrictRedis()
    ps = client.pubsub()
    ps.subscribe("joystick")
    for message in ps.listen():
        if message["type"] != "message":
            continue
        payload = json.loads(message["data"].decode("utf-8"))
        msg = lambda x: x
        msg.type = payload["type"]
        msg.data = payload["data"]
        print(message)
        callback(msg)

if __name__ == "__main__":
    init()
