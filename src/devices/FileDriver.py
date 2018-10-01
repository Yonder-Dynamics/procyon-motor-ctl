import sys

from devices.Driver import DriverWriteException
from util.Keychain import Keychain

ID = "files"

KEYS = Keychain(
    "file_driver_keys",
    INFILE="infile",
    OUTFILE="outfile",
)

def create(config):
    infile = config.get(KEYS.INFILE)
    outfile = config.get(KEYS.OUTFILE)
    return FileDriver(infile, outfile)


class FileDriver:
    """Driver for printing serialized input to a file.
    """
    def __init__(self, infile, outfile):
        self.infile = infile
        self.input = None

        self.outfile = outfile
        self.output = None

    def send(self, message):
        self.output.write(message)

    def receive(self):
        # return self.input.read()
        pass

    def connect(self):
        if self.infile is not None:
            self.input = open(self.infile)
        else:
            self.input = sys.stdin

        if self.outfile is not None:
            self.output = open(self.outfile)
        else:
            self.output = sys.stdout

    def fix(self):
        pass

    def reset(self):
        self.input.truncate()
        self.output.truncate()

    def close(self):
        self.input.close()
        self.output.close()

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *args):
        self.close()
        return