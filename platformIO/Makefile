# Uncomment lines below if you have problems with $PATH
# SHELL := C:/windows/System32/WindowsPowerShell/v1.0/powershell.EXE
# SHELL := /bin/bash
# PATH := /usr/local/bin:$(PATH)

all: build db upload monitor

build:
	platformio -c vim run

monitor:
	platformio -c vim device monitor

db:
	platformio -c vim run --target compiledb
# 	cp .pio/build/nodemcuv2/compile_commands.json .

upload:
	platformio -c vim run --target upload

clean:
	platformio -c vim run --target clean

program:
	platformio -c vim run --target program

uploadfs:
	platformio -c vim run --target uploadfs

update:
	platformio -c vim update
