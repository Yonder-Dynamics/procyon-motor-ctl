#!/usr/bin/env python
import rospy
from rover_ctl.msg import MotorCMD
from std_msgs.msg import Bool
from serial import Serial
import time, math

#addr = "00:14:03:05:F1:F5"
MSG_RATE  = 15 #in Hz
MSG_PER = 1./MSG_RATE
MAX_TURNING_RADIUS = 10
TIMEOUT = 2
# Always use serial ports like this because they don't change
serial_port = "/dev/serial/by-id/usb-Arduino__www.arduino.cc__0042_8543034393735141E052-if00"

def openSerial():
    global s
    s = Serial(serial_port, 19200)
    time.sleep(1)

# Open serial. DO NOT REMOVE DELAY
print("Opening serial")
openSerial()
print("Started listening")

last_message_send = 0
is_stopped = False

# data: [int]
def makeSerialMsg(data):
    serialMsg = b'{},' * len(data) + b'\n'
    serialMsg = serialMsg.format(*data)
    return serialMsg

def kill_callback(msg):
    global is_stopped
    is_stopped = msg.data
    s.write(makeSerialMsg([0]*6))

def callback(msg):
    global s, is_stopped, last_message_send
    # Kill switch
    if is_stopped:
        return

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
    #print(serialMsg)
    try:
        s.write(serialMsg)
    except:
        openSerial()
        print("Looking for serial port: %s ..." % serial_port)
    #print(s.readline())

def init():
    global last_message_send
    rospy.init_node("serial_motor_driver", anonymous=True)
    rospy.Subscriber("/motor_ctl", MotorCMD, callback)
    rospy.Subscriber("/killswitch", Bool, kill_callback)
    # Check if still recieving messages
    #while not rospy.is_shutdown():
    #    if last_message_send > TIMEOUT:
    #        s.write(makeSerialMsg([0] * 6))
    #    rospy.spin_once()
    rospy.spin()
    s.write(makeSerialMsg([0] * 6))
    s.close()

if __name__ == "__main__":
    init()
