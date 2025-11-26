#ifndef MCP41XXX_H
#define MCP41XXX_H

// MCP41XXX Commands (single pot)
#define MCP41_CMD_WRITE    0x11  // Write data to wiper
#define MCP41_CMD_SHUTDOWN 0x21  // Shutdown wiper

// MCP42XXX Commands (dual pot)
#define MCP42_CMD_WRITE_POT0  0x11  // Write to pot 0
#define MCP42_CMD_WRITE_POT1  0x12  // Write to pot 1
#define MCP42_CMD_WRITE_BOTH  0x13  // Write to both pots
#define MCP42_CMD_SHUTDOWN_POT0 0x21  // Shutdown pot 0
#define MCP42_CMD_SHUTDOWN_POT1 0x22  // Shutdown pot 1
#define MCP42_CMD_SHUTDOWN_BOTH 0x23  // Shutdown both pots

// Wiper positions (0-255 for 8-bit models)
#define MCP41_MIN   0
#define MCP41_MAX   255
#define MCP41_MID   128

// Single pot (MCP41xxx)
typedef struct {
  uint8_t cs_pin;
  uint8_t current_pos;
} MCP41;

// Dual pot (MCP42xxx)
typedef struct {
  uint8_t cs_pin;
  uint8_t current_pos0;  // Pot 0 position
  uint8_t current_pos1;  // Pot 1 position
} MCP42;

// MCP41xxx functions (single pot)
void mcp41_init(MCP41 *pot, uint8_t cs_pin);
void mcp41_set_wiper(MCP41 *pot, uint8_t value);
void mcp41_increment(MCP41 *pot);
void mcp41_decrement(MCP41 *pot);
void mcp41_shutdown(MCP41 *pot);
uint8_t mcp41_get_position(MCP41 *pot);

// MCP42xxx functions (dual pot)
void mcp42_init(MCP42 *pot, uint8_t cs_pin);
void mcp42_set_pot0(MCP42 *pot, uint8_t value);
void mcp42_set_pot1(MCP42 *pot, uint8_t value);
void mcp42_set_both(MCP42 *pot, uint8_t val0, uint8_t val1);
void mcp42_increment_pot0(MCP42 *pot);
void mcp42_decrement_pot0(MCP42 *pot);
void mcp42_increment_pot1(MCP42 *pot);
void mcp42_decrement_pot1(MCP42 *pot);
void mcp42_shutdown(MCP42 *pot);
uint8_t mcp42_get_position0(MCP42 *pot);
uint8_t mcp42_get_position1(MCP42 *pot);

#endif
