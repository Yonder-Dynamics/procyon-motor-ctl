from devices.Driver import DriverWriteException

ID = "hash_delimited"

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
    return HashDelimitedProtocol()


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
    def __init__(self, driver=None):
        self.driver = driver

    def setup(self, driver):
        self.driver = driver

    def send(self, msg_type_str, data):
        msg_type = MSG_TYPES[msg_type_str]
        msg = format_message(msg_type, data)
        # attempt to send message
        while not attempt_write(self.driver.send, msg):
            self.driver.fix()
        self.driver.receive()