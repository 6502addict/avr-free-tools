# Top-level Makefile - builds all AVR projects

.PHONY: all clean help

# Default target - build everything
all:
	@echo "Building all AVR projects..."
	$(MAKE) -C avr-tiny-asm   all-chips
	$(MAKE) -C avr-mega-c     all-chips
	$(MAKE) -C avr-mega-asm   all-chips
	$(MAKE) -C avr-mega-c-asm all-chips
	@echo "All projects built successfully"

# Clean all projects
clean:
	@echo "Cleaning all projects..."
	$(MAKE) -C avr-tiny-asm   clean
	$(MAKE) -C avr-mega-c     clean
	$(MAKE) -C avr-mega-asm   clean
	$(MAKE) -C avr-mega-c-asm clean
	@echo "All projects cleaned"

help:
	@echo "AVR Template Projects"
	@echo "  make all   - Build all projects"
	@echo "  make clean - Clean all projects"
