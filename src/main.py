#!/usr/bin/env python3
import argparse
import json

from util.Keychain import Keychain

from protocols import HashDelimited
from devices import (
    SerialDriver,
    FileDriver,
)
from inputs import Redis

KEYS = Keychain(
    "driver_script_keys",
    BAUDRATE="baudrate",
    PROTOCOL=Keychain(
        "protocol",
        TYPE="type",
    ),
    DRIVER=Keychain(
        "driver",
        TYPE="type",
    ),
    INPUT=Keychain(
        "input",
        TYPE="type",
    ),
)


class ConfigError(ValueError):
    pass


def convert_message(payload):
    dct = json.loads(payload)
    msg_type = dct["type"]
    msg_data = dct["data"]
    return msg_type, msg_data


def find_module(modules, type_name, config):
    for module in modules:
        if type_name == module.ID:
            return module.create(config)
    raise ConfigError("Invalid type {}.".format(type_name))


def main(config):
    # stack:
    #   source: recieves input and writes it to an observer object
    #   converter: converts input to format expected by protocol
    #   protocol: converts input to serializable format to write to device
    #   driver: serializes input and writes it to hardware
    protocol = None
    driver = None
    source = None
    converter = None

    driver_type = config[KEYS.DRIVER][KEYS.DRIVER.TYPE]
    protocol_type = config[KEYS.PROTOCOL][KEYS.PROTOCOL.TYPE]
    source_type = config[KEYS.INPUT][KEYS.INPUT.TYPE]
    
    # set up the device driver
    drivers = [SerialDriver, FileDriver]
    driver = find_module(drivers, driver_type, config[KEYS.DRIVER])
    driver.connect()

    # set up the motor protocol
    protocols = [HashDelimited]
    protocol = find_module(protocols, protocol_type, config[KEYS.PROTOCOL])
    protocol.setup(driver)

    # set up a converter for input format to protocol input format
    converter = lambda msg: protocol.send(*convert_message(msg))

    sources = [Redis]
    source = find_module(sources, source_type, config[KEYS.INPUT])

    print("Driver: {}\nProtocol: {}\nSource: {}".format(
        driver_type, protocol_type, source_type
    ))

    source.begin(observer=converter)  # start listening to the input

def entrypoint(cmdline):
    with open(cmdline.config,  "r") as file:
        config = json.load(file)
    main(config)

def configure_parser(parser):
    parser.add_argument(
        "config",
        help="Configuration JSON file for the driver.",
    )

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    configure_parser(parser)
    try:
        entrypoint(parser.parse_args())
    except ConfigError as e:
        print("Bad configuration: {}".format(e))