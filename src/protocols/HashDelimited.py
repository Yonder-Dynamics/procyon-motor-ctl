import time
from icecream import ic

from devices.Driver import DriverWriteException
from util.Keychain import Keychain

ID = "hash_delimited"

KEYS = Keychain(
    "protocol",
    MSG_RATE="msg_rate",
)

MSG_TYPES = {
    "drive": 0,
    "motors": 0,
    "arm": 1,
    "drill": 2,
    "terr": 3,
    "manual": 4,
}

MSG_FORMAT = "#{type}#{csv}\n"

def create(config):
    msg_rate = config[KEYS.MSG_RATE]
    return HashDelimitedProtocol(msg_rate=msg_rate)


def validate_message(msg_type, data):
    if any([d > 255 for d in data]):  # if we get invalid data, zero it
        return [0 for d in data]
    return data


def get_message_priority(msg_type, data):
    if msg_type == 0: # drive messages
        if not any(data): # drive kill messages
            return 1
    return 0


def format_message(msg_type, data):
    return MSG_FORMAT.format(
        type=msg_type,
        csv=','.join([str(d) for d in data]),
    )

def attempt_write(func, *args, **kwargs):
    try:
        func(*args, **kwargs)
    except DriverWriteException:
        return False
    return True


class HashDelimitedProtocol:
    def __init__(self, driver=None, msg_rate=20):
        self.driver = driver
        self.msg_rate = msg_rate
        self.msg_per = 1 / float(msg_rate)
        self.buffer = [-1, None]
        self.last_send = time.time()

    def setup(self, driver):
        self.driver = driver

    def send(self, msg_type_str, data):
        msg_type = MSG_TYPES[msg_type_str]
        data = validate_message(msg_type, data)
        priority = get_message_priority(msg_type, data)
        
        if priority >= self.buffer[0]:
            self.buffer[0] = priority
            self.buffer[1] = format_message(msg_type, data)

        curr = time.time()
        if curr - self.last_send > self.msg_per or priority >= 1:
            msg = self.buffer[1]
            # attempt to send message
            while not attempt_write(self.driver.send, msg):
                self.driver.fix()
            self.last_send = curr
            self.buffer[0] = -1
            self.driver.receive()
