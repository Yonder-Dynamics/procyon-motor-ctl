import serial
from devices.Driver import DriverWriteException
from util.Keychain import Keychain

ID = "serial"

KEYS = Keychain(
    "serial_driver_keys",
    BAUDRATE="baudrate",
    PORT="port",
    ENCODING="encoding",
)

DEFAULT_ENCODING = 'utf-8'


def create(config):
    baudrate = config[KEYS.BAUDRATE]
    port = config[KEYS.PORT]
    encoding = config.get(KEYS.ENCODING, DEFAULT_ENCODING)

    return SerialDriver(port, baudrate, encoding)


class SerialDriver:
    """Driver for a controlling device connected via USB.
    """
    def __init__(self, port, baudrate, encoding=DEFAULT_ENCODING):
        self.port = port
        self.baudrate = baudrate
        self.encoding = encoding
        self.conn = None

    def send(self, message):
        try:
            self.conn.write(bytes(message, self.encoding))
        except serial.SerialTimeoutException:
            raise DriverWriteException

    def receive(self):
        return self.conn.read(self.conn.inWaiting()).decode(self.encoding)

    def connect(self):
        self.conn = serial.Serial(self.port, self.baudrate)

    def fix(self):
        self.connect()  # try to fix a connection issue by reopening the port

    def reset(self):
        self.conn.reset_input_buffer()
        self.conn.reset_output_buffer()

    def close(self):
        self.conn.close()

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *args):
        self.close()
        return
