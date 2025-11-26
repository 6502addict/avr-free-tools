# Master Makefile for projects
# Only builds subdirectories listed in SUBDIRS

SUBDIRS = libraries ds1302-test dskbrowser font-transform-test ili948x-test joystick-test mcp41xxx-test mega-freqgen mega-ne567 ssd1306-test ssd1680-test \
	  tiny-blink tiny-ne555 tiny-calibrate tiny-freqgen mega-ne567 tiny-fsk-mod tiny-fsk-demod wheel-test

.PHONY: all libraries projects clean all-clean install-all

# Build everything
all: libraries projects

# Build and install all libraries
libraries:
	$(MAKE) -C libraries install-all

# Build all projects (excluding libraries)
projects: libraries
	@for dir in $(filter-out libraries,$(SUBDIRS)); do \
		echo "Building $$dir..."; \
		$(MAKE) -C $$dir all-mcus || exit 1; \
	done

# Clean build artifacts
clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

# Deep clean - remove ALL build artifacts
all-clean:
	@echo "Removing all build artifacts..."
	@for dir in $(SUBDIRS); do \
		echo "Cleaning $$dir..."; \
		$(MAKE) -C $$dir clean 2>/dev/null || true; \
		rm -f $$dir/*.elf $$dir/*.hex $$dir/*.o $$dir/*.map $$dir/*~; \
	done
	@echo "Removing installed libraries and headers..."
	rm -rf ../lib/*.a
	rm -rf ../include/*.h
	@echo "All clean!"

# Install all libraries
install-all:
	$(MAKE) -C libraries install-all
