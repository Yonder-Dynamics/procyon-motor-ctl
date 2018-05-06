#!/usr/bin/env python
import rospy
from rover_ctl.msg import MotorCMD
from serial import Serial
import time, math

#addr = "00:14:03:05:F1:F5"
MSG_RATE  = 15 #in Hz
MSG_PER = 1./MSG_RATE
MAX_TURNING_RADIUS = 10
TIMEOUT = 2

# Open serial. DO NOT REMOVE DELAY
#s = Serial("/dev/ttyACM0", 9600)
s = Serial("/dev/ttyACM0", 19200)
time.sleep(1)
last_message_send = 0

# data: [int]
def makeSerialMsg(data):
    serialMsg = b'{},' * len(len(data)) + b'\n'
    serialMsg = serialMsg.format(*data)
    return serialMsg

def callback(msg):
    global last_message_send
    if time.time() - last_message_send < MSG_PER:
        return
    last_message_send = time.time()

    # Check for out of bounds ints
    for x in msg.data:
        if x > 255:
            s.write(makeSerialMsg([0] * 6))
            return

    #Send
    serialMsg = makeSerialMsg(msg.data)
    print(serialMsg)
    s.write(serialMsg)
    #print(s.readline())

def init():
    global last_message_send
    rospy.init_node("serial_motor_driver", anonymous=True)
    rospy.Subscriber("/motor_ctl", MotorCMD, callback)
    # Check if still recieving messages
    while not rospy.is_shutdown():
        if last_message_send > TIMEOUT:
            s.write(makeSerialMsg([0] * 6))
        rospy.spinOnce()
    s.write(makeSerialMsg([0] * 6))
    s.close()

if __name__ == "__main__":
    init()

