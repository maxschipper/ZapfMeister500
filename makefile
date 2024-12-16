# arduino-cli upload -p /dev/cu.usbserial-14140 --fqbn arduino:avr:uno .
# arduino-cli compile --fqbn arduino:avr:uno /pfad/zum/projekt

BOARD_FQBN=arduino:avr:uno
PORT=/dev/cu.usbserial-*
# PORT=/dev/cu.usbserial-14140
PROJECT_DIR=$(shell pwd)  # Aktuelles Verzeichnis als Projektpfad

build:
	arduino-cli compile --fqbn $(BOARD_FQBN) $(PROJECT_DIR)

flash: build
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(PROJECT_DIR)

buildout:
	arduino-cli compile --fqbn $(BOARD_FQBN) --output-dir ./build $(PROJECT_DIR)

clean:
	rm -r ./build/

help:
	@echo "Verfügbare Befehle:"
	@echo "  make compile   - Kompiliert das Programm"
	@echo "  make flash     - Flasht das Programm auf das Board (führt auch compile aus)"


