
DRIVER = src/main.py
CONFIG = config.json

.PHONY: all driver

all: driver

driver:
	@ $(DRIVER) $(CONFIG)

