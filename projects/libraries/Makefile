# Top-level Makefile for AVR libraries

SUBDIRS = spi timer sdcard fatfs i2c uart-mega uart-tiny ds1302 font-transform ssd1306 ssd1680 ili948x  bme280 wheel  mcp41xxx 

.PHONY: all install install-all clean all-mcus $(SUBDIRS)

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir all || exit 1; \
	done

install:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install || exit 1; \
	done

install-all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install-all || exit 1; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean || exit 1; \
	done

all-mcus:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir all-mcus || exit 1; \
	done


