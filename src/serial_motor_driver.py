#!/usr/bin/env python
import rospy
from rover_ctl.msg import MotorCMD
from serial import Serial
import time, math

#addr = "00:14:03:05:F1:F5"
MSG_RATE  = 15 #in Hz
MSG_PER = 1./MSG_RATE
last_message_send = 0
MAX_TURNING_RADIUS = 10

# Open serial. DO NOT REMOVE DELAY
#s = Serial("/dev/ttyACM0", 9600)
s = Serial("/dev/ttyACM0", 19200)
time.sleep(1)

class Motor:
    def __init__(self, id, x, y, maxSpeed, isFlipped):
        self.id = id
        self.x = x
        self.y = y
        self.maxSpeed = maxSpeed
        self.isFlipped = isFlipped

    def getTurningSpeed(self, turningRadius, turningSpeed):
        #return turningSpeed * (turningRadius - self.x) / (self.x**2+self.y**2)
        sign = 1 if self.x > 0 else -1
        return -sign*turningRadius*turningSpeed

    def getMotorSpeed(self, turningRadius, turningSpeed, maxTurningSpeed, forwardVel):
        maxTurningSpeed = 1 if maxTurningSpeed == 0 else maxTurningSpeed
        return forwardVel + self.getTurningSpeed(turningRadius, turningSpeed)

drivetrain = []
drivetrain.append(Motor(0,-0.4218, 0.53975, 255, False))
drivetrain.append(Motor(1,-0.4218, 0, 255, False))
drivetrain.append(Motor(2,-0.4080, -0.381, 255, False))
drivetrain.append(Motor(3,0.4080, -0.381, 255, False))
drivetrain.append(Motor(4,0.4218, 0, 255, False))
drivetrain.append(Motor(5,0.4218, 0.53975, 255, False))

wheelBaseWidth = 0.50

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
            break

    #Send
    serialMsg = makeSerialMsg(msg.data)
    print(serialMsg)
    s.write(serialMsg)
    #print(s.readline())

def init():
    rospy.init_node("serial_motor_driver", anonymous=True)
    rospy.Subscriber("/motor_ctl", MotorCMD, callback)
    rospy.spin()
    s.write(makeSerialMsg([0] * 6))
    s.close()

if __name__ == "__main__":
    init()

