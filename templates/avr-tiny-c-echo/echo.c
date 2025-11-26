// main.c
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <uart-tiny.h>


//#define LED_PIN     PB1
#define EEPROM_OSCCAL_ADDR 0

const char hex_digits[]  PROGMEM = "0123456789ABCDEF";
const char str_title[]   PROGMEM = "OSCCAL Calibration\r\n";
const char str_eeprom[]  PROGMEM = "Eeprom OSCCAL: 0x";
const char str_current[] PROGMEM = "Current OSCCAL: 0x";
const char str_crlf[]    PROGMEM = "\r\n";

static void uart_tx_string_P(const char *str) {
    char c;
    while ((c = pgm_read_byte(str++))) {
        uart_tx(c);
    }
}

void print_hex(uint8_t val) {
    uart_tx(pgm_read_byte(&hex_digits[val >> 4]));
    uart_tx(pgm_read_byte(&hex_digits[val & 0x0F]));
}

int main(void) {
  //  eeprom_write_byte((uint8_t*)0, 0x5C);
  uint8_t cal = eeprom_read_byte((uint8_t*)EEPROM_OSCCAL_ADDR);
  if (cal != 0xff) {
    OSCCAL = cal;
  } else {
    OSCCAL = 0x5C;
  }
  _delay_ms(1000);

  uart_init();
  
  uart_tx_string_P(str_title);
  uart_tx_string_P(str_eeprom);
  print_hex(cal);
  uart_tx_string_P(str_crlf);
  uart_tx_string_P(str_current);
  print_hex(OSCCAL);
  uart_tx_string_P(str_crlf);
  while (1) {
    uart_tx(uart_rx());
  }
  return 0;
}
