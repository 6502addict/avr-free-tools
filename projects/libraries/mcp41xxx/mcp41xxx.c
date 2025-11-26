#include <avr/io.h>
#include <stdint.h>
#include <spi.h>
#include <mcp41xxx.h>

static uint8_t spi_initialized = 0;

// Helper to initialize SPI (shared by both)
static void ensure_spi_init(void) {
  if (!spi_initialized) {
    spi_init(16, 0, 0);  // 1MHz, CPOL=0, CPHA=0
    spi_initialized = 1;
  }
}

// Helper to send command
static void send_command(uint8_t cs_pin, uint8_t cmd, uint8_t data) {
  PORTB &= ~(1 << cs_pin);  // CS low
  spi_transfer(cmd);
  spi_transfer(data);
  PORTB |= (1 << cs_pin);   // CS high
}

// ========== MCP41xxx (Single Pot) ==========

void mcp41_init(MCP41 *pot, uint8_t cs_pin) {
  pot->cs_pin = cs_pin;
  pot->current_pos = MCP41_MID;
  
  DDRB |= (1 << cs_pin);
  PORTB |= (1 << cs_pin);
  
  ensure_spi_init();
  mcp41_set_wiper(pot, MCP41_MID);
}

void mcp41_set_wiper(MCP41 *pot, uint8_t value) {
  pot->current_pos = value;
  send_command(pot->cs_pin, MCP41_CMD_WRITE, value);
}

void mcp41_increment(MCP41 *pot) {
  if (pot->current_pos < MCP41_MAX)
    mcp41_set_wiper(pot, pot->current_pos + 1);
}

void mcp41_decrement(MCP41 *pot) {
  if (pot->current_pos > MCP41_MIN)
    mcp41_set_wiper(pot, pot->current_pos - 1);
}

void mcp41_shutdown(MCP41 *pot) {
  send_command(pot->cs_pin, MCP41_CMD_SHUTDOWN, 0x00);
}

uint8_t mcp41_get_position(MCP41 *pot) {
  return pot->current_pos;
}

// ========== MCP42xxx (Dual Pot) ==========

void mcp42_init(MCP42 *pot, uint8_t cs_pin) {
  pot->cs_pin = cs_pin;
  pot->current_pos0 = MCP41_MID;
  pot->current_pos1 = MCP41_MID;
  
  DDRB |= (1 << cs_pin);
  PORTB |= (1 << cs_pin);
  
  ensure_spi_init();
  mcp42_set_both(pot, MCP41_MID, MCP41_MID);
}

void mcp42_set_pot0(MCP42 *pot, uint8_t value) {
  pot->current_pos0 = value;
  send_command(pot->cs_pin, MCP42_CMD_WRITE_POT0, value);
}

void mcp42_set_pot1(MCP42 *pot, uint8_t value) {
  pot->current_pos1 = value;
  send_command(pot->cs_pin, MCP42_CMD_WRITE_POT1, value);
}

void mcp42_set_both(MCP42 *pot, uint8_t val0, uint8_t val1) {
  pot->current_pos0 = val0;
  pot->current_pos1 = val1;
  
  // Send to pot0 first, then pot1
  send_command(pot->cs_pin, MCP42_CMD_WRITE_POT0, val0);
  send_command(pot->cs_pin, MCP42_CMD_WRITE_POT1, val1);
}

void mcp42_increment_pot0(MCP42 *pot) {
  if (pot->current_pos0 < MCP41_MAX)
    mcp42_set_pot0(pot, pot->current_pos0 + 1);
}

void mcp42_decrement_pot0(MCP42 *pot) {
  if (pot->current_pos0 > MCP41_MIN)
    mcp42_set_pot0(pot, pot->current_pos0 - 1);
}

void mcp42_increment_pot1(MCP42 *pot) {
  if (pot->current_pos1 < MCP41_MAX)
    mcp42_set_pot1(pot, pot->current_pos1 + 1);
}

void mcp42_decrement_pot1(MCP42 *pot) {
  if (pot->current_pos1 > MCP41_MIN)
    mcp42_set_pot1(pot, pot->current_pos1 - 1);
}

void mcp42_shutdown(MCP42 *pot) {
  send_command(pot->cs_pin, MCP42_CMD_SHUTDOWN_BOTH, 0x00);
}

uint8_t mcp42_get_position0(MCP42 *pot) {
  return pot->current_pos0;
}

uint8_t mcp42_get_position1(MCP42 *pot) {
  return pot->current_pos1;
}
