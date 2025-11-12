#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include "ds1302.h"

#define INPUT  0
#define OUTPUT 1

// RST handling
static inline void ds1302_rst_mode(uint8_t value) {
  if (value) 							      
    DS1302_DDR |= (1 << DS1302_RST);				
  else 							
    DS1302_DDR  &= ~(1 << DS1302_RST);				
}

static inline void ds1302_rst_write(uint8_t value) {
  if (value) 							      
    DS1302_PORT  |= (1 << DS1302_RST);				
  else							
    DS1302_PORT  &= ~(1 << DS1302_RST);				
}    

// SCLK handling
static inline void ds1302_sclk_mode(uint8_t value) {
  if (value) 							     
    DS1302_DDR  |= (1 << DS1302_SCLK);				
  else 							
    DS1302_DDR  &= ~(1 << DS1302_SCLK);				
}

static inline void ds1302_sclk_write(uint8_t value) {
  if (value) 
    DS1302_PORT |= (1 << DS1302_SCLK);				
  else
    DS1302_PORT &= ~(1 << DS1302_SCLK);				
}    

// IO handling
static inline void ds1302_io_mode(uint8_t value) {
  if (value) 							      
    DS1302_DDR  |= (1 << DS1302_IO);				
  else {							
    DS1302_DDR  &= ~(1 << DS1302_IO);				
  }								
}

static inline uint8_t ds1302_io_read() {
  uint8_t data = ((DS1302_PIN & (1 << DS1302_IO)) ? 1 : 0);
  return data;
}

static inline void ds1302_io_write(uint8_t value) {
  if (value)
    DS1302_PORT |= (1 << DS1302_IO);				
  else 
    DS1302_PORT &= ~(1 << DS1302_IO);				
}    

// Initialize DS1302
void ds1302_init(void) {
  ds1302_rst_mode(INPUT);
  ds1302_sclk_mode(INPUT);
  ds1302_io_mode(INPUT);
}

// helper function to send a byte
// from datasheet:
// Following the eight SCLK cycles that input a write command byte, a data byte is input on the rising edge
// of the next eight SCLK cycles. Additional SCLK cycles are ignored should they inadvertently occur.
// Data is input starting with bit 0.
static inline void ds1302_data_input(uint8_t byte, bool reading) {
  for (uint8_t i = 0; i < 8; i++) {
    ds1302_io_write(byte & 0x01);
    _delay_us(1);
    ds1302_sclk_write(1);    
    _delay_us(1);
    // set to input mode if reading
    if ((i == 7) && reading) 
      ds1302_io_mode(INPUT);
    ds1302_sclk_write(0);    
    _delay_us(1);
    byte >>= 1;
  }
}

// helper function to receive a byte
// from datasheet:
// Following the eight SCLK cycles that input a write command byte, a data byte is input on the rising edge
// of the next eight SCLK cycles. Additional SCLK cycles are ignored should they inadvertently occur.
// Data is input starting with bit 0.
static inline uint8_t ds1302_data_output(void) {
  uint8_t byte = 0;
  
  for (uint8_t i = 0; i < 8; i++) {
    byte |= (ds1302_io_read() << i);
    ds1302_sclk_write(1);
    _delay_us(1);
    ds1302_sclk_write(0);
    _delay_us(1);
  }
  return byte;
}

static inline void ds1302_cmd_start(uint8_t cmd) {
  ds1302_rst_write(0);
  ds1302_rst_mode(OUTPUT);
  ds1302_sclk_write(0);
  ds1302_sclk_mode(OUTPUT);
  ds1302_io_mode(OUTPUT);  
  ds1302_rst_write(1);
  _delay_us(4);
  ds1302_data_input(cmd, (cmd & DS1302_READ_BIT) == DS1302_READ_BIT);
}  

static inline void ds1302_cmd_end() {
  ds1302_rst_write(0);  
  _delay_us(4);
}

// Write a byte to DS1302
static inline void ds1302_write(uint8_t addr, uint8_t data) {
  ds1302_cmd_start(addr);
  ds1302_data_input(data, false);
  ds1302_cmd_end();
}

// Read a byte from DS1302
static inline uint8_t ds1302_read(uint8_t addr) {
  ds1302_cmd_start(addr);
  uint8_t data = ds1302_data_output();
  ds1302_cmd_end();
  return data;
}

// Write a ram  byte to DS1302
void ds1302_write_ram(uint8_t addr, uint8_t data) {
  ds1302_write(DS1302_CMD_BIT | DS1302_RAM_BIT | (addr << 1), data);
}

// Read a ram byte from DS1302
uint8_t ds1302_read_ram(uint8_t addr) {
  return ds1302_read(DS1302_CMD_BIT | DS1302_RAM_BIT | (addr << 1) | DS1302_READ_BIT);
}

// Write a rtc byte to DS1302
void ds1302_write_rtc(uint8_t addr, uint8_t data) {
  uint8_t cmdbyte = DS1302_CMD_BIT | (addr << 1);
  ds1302_write(cmdbyte, data);
}

// Read a rtc byte from DS1302
uint8_t ds1302_read_rtc(uint8_t addr) {
  return ds1302_read(DS1302_CMD_BIT | (addr << 1) | DS1302_READ_BIT | (addr << 1) | 1);
}
