# arduino-cli upload -p /dev/cu.usbserial-14140 --fqbn arduino:avr:uno .
# arduino-cli compile --fqbn arduino:avr:uno /pfad/zum/projekt


# Makefile für Arduino CLI

# Variablen
BOARD_FQBN=arduino:avr:uno
PORT=/dev/cu.usbserial-14140
PROJECT_DIR=$(shell pwd)  # Aktuelles Verzeichnis als Projektpfad

# Ziel: compile - Kompiliert den Sketch
compile:
	arduino-cli compile --fqbn $(BOARD_FQBN) $(PROJECT_DIR)

# Ziel: flash - Flasht den Sketch auf das Board
flash: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD_FQBN) $(PROJECT_DIR)

# Ziel: clean - Entfernt kompilierte Dateien
clean:
	rm -rf $(PROJECT_DIR)/build

# Hilfsziel: Zeigt verfügbare Befehle
help:
	@echo "Verfügbare Befehle:"
	@echo "  make compile   - Kompiliert den Sketch"
	@echo "  make flash     - Flasht den Sketch auf das Board (führt auch compile aus)"
	@echo "  make clean     - Entfernt kompilierte Dateien"


