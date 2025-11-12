// osccal_calibrate.c
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "uart_tiny.h"

// EEPROM address to store calibrated value
#define EEPROM_OSCCAL_ADDR 0

// All strings in flash
const char str_title[] PROGMEM = "OSCCAL Calibration\r\n";
const char str_current[] PROGMEM = "Current OSCCAL: 0x";
const char str_eeprom[] PROGMEM = "EEPROM OSCCAL: 0x";
const char str_commands[] PROGMEM = "\r\nCommands:\r\n";
const char str_cmd_plus[] PROGMEM = "  + : increase OSCCAL\r\n";
const char str_cmd_minus[] PROGMEM = "  - : decrease OSCCAL\r\n";
const char str_cmd_save[] PROGMEM = "  s : save to EEPROM\r\n";
const char str_cmd_show[] PROGMEM = "  r : show current\r\n";
const char str_prompt[] PROGMEM = "\r\n> ";
const char str_osccal[] PROGMEM = "OSCCAL: 0x";
const char str_saved[] PROGMEM = "Saved 0x";
const char str_to_eeprom[] PROGMEM = " to EEPROM\r\n";
const char str_crlf[] PROGMEM = "\r\n";
const char hex_digits[] PROGMEM = "0123456789ABCDEF";

// Print string from PROGMEM byte by byte
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
  uint8_t stored = eeprom_read_byte((uint8_t*)EEPROM_OSCCAL_ADDR);
  if (stored != 0xff) {
    OSCCAL = stored;
  } else {
    OSCCAL = 0x5C;
  }
  _delay_ms(1000);
  
  
  uart_init();
  _delay_ms(100);
  
  //  while(1) {
  //    uart_tx(uart_rx());
  //}
  uart_tx_string_P(str_title);
  uart_tx_string_P(str_current);
  print_hex(OSCCAL);
  uart_tx_string_P(str_crlf);
  
  // Read stored value from EEPROM
  //uint8_t stored = eeprom_read_byte((uint8_t*)EEPROM_OSCCAL_ADDR);
  if (stored != 0xFF) {
    uart_tx_string_P(str_eeprom);
    print_hex(stored);
    uart_tx_string_P(str_crlf);
  }
  
  uart_tx_string_P(str_commands);
  uart_tx_string_P(str_cmd_plus);
  uart_tx_string_P(str_cmd_minus);
  uart_tx_string_P(str_cmd_save);
  uart_tx_string_P(str_cmd_show);
  uart_tx_string_P(str_prompt);
  
  while (1) {
    uint8_t c = uart_rx();
    uart_tx(c);
    uart_tx_string_P(str_crlf);
    
    if (c == '+') {
      OSCCAL++;
      uart_tx_string_P(str_osccal);
      print_hex(OSCCAL);
      uart_tx_string_P(str_crlf);
    } else if (c == '-') {
      OSCCAL--;
      uart_tx_string_P(str_osccal);
      print_hex(OSCCAL);
      uart_tx_string_P(str_crlf);
    } else if (c == 's') {
      eeprom_write_byte((uint8_t*)EEPROM_OSCCAL_ADDR, OSCCAL);
      uart_tx_string_P(str_saved);
      print_hex(OSCCAL);
      uart_tx_string_P(str_to_eeprom);
    } else if (c == 'r') {
      uart_tx_string_P(str_osccal);
      print_hex(OSCCAL);
      uart_tx_string_P(str_crlf);
    }
    
    uart_tx_string_P(str_prompt);
  }
  return 0;
}
