from serial import Serial

class SerialDriver:
    """Driver for a controlling device connected via USB.
    """
    def __init__(self, port):
        self.port = port
        self.conn = None
        self.baudrate = None

    def begin(self, baudrate):
        self.baudrate = baudrate
        self.conn = Serial(self.port, self.baudrate)

    def send(self, message):

    