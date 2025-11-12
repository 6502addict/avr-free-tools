// main.c
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "freq_gen_tiny.h"
#include "uart_tiny.h"

#define CMD_BUFFER_SIZE 16

// Send string via UART
static void uart_tx_string(const char *str) {
    while (*str) {
        uart_tx(*str++);
    }
}

// Parse frequency from command string
// Returns 0 if invalid
static uint16_t parse_frequency(char *cmd) {
    uint16_t freq = 0;
    
    // Skip whitespace
    while (*cmd == ' ') cmd++;
    
    // Parse digits
    while (isdigit(*cmd)) {
        freq = freq * 10 + (*cmd - '0');
        cmd++;
    }
    
    return freq;
}

// Check if command is "stop"
static uint8_t is_stop_command(char *cmd) {
    // Skip whitespace
    while (*cmd == ' ') cmd++;
    
    // Convert to lowercase and compare
    if ((cmd[0] == 's' || cmd[0] == 'S') &&
        (cmd[1] == 't' || cmd[1] == 'T') &&
        (cmd[2] == 'o' || cmd[2] == 'O') &&
        (cmd[3] == 'p' || cmd[3] == 'P')) {
        return 1;
    }
    return 0;
}

// Process command
static void process_command(char *cmd) {
    if (is_stop_command(cmd)) {
        freq_gen_stop();
        uart_tx_string("STOPPED\r\n");
    } else {
        uint16_t freq = parse_frequency(cmd);
        if (freq > 0 && freq < 10000) {
            freq_gen_set(freq);
            uart_tx_string("OK ");
            
            // Echo frequency back
            char buf[8];
            uint8_t i = 0;
            if (freq == 0) {
                buf[i++] = '0';
            } else {
                uint16_t temp = freq;
                uint8_t digits = 0;
                while (temp > 0) {
                    digits++;
                    temp /= 10;
                }
                i = digits;
                buf[i] = '\0';
                while (digits > 0) {
                    buf[--digits] = (freq % 10) + '0';
                    freq /= 10;
                }
            }
            buf[i++] = 'H';
            buf[i++] = 'z';
            buf[i++] = '\0';
            
            uart_tx_string(buf);
            uart_tx_string("\r\n");
        } else {
            uart_tx_string("ERROR\r\n");
        }
    }
}

int main(void) {
    char cmd_buffer[CMD_BUFFER_SIZE];
    uint8_t cmd_index = 0;
    uint8_t c;
    
    uart_init();
    _delay_ms(100);
    
    uart_tx_string("FreqGen v1.0\r\n");
    uart_tx_string("Commands: <freq> or stop\r\n");
    uart_tx_string("> ");
    
    while (1) {
        c = uart_rx();
        
        // Echo character
        uart_tx(c);
        
        if (c == '\r' || c == '\n') {
            uart_tx_string("\r\n");
            
            if (cmd_index > 0) {
                cmd_buffer[cmd_index] = '\0';
                process_command(cmd_buffer);
                cmd_index = 0;
            }
            
            uart_tx_string("> ");
        } else if (c == 8 || c == 127) {  // Backspace or DEL
            if (cmd_index > 0) {
                cmd_index--;
                uart_tx_string(" \b");  // Erase character
            }
        } else if (cmd_index < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_index++] = c;
        }
    }
    
    return 0;
}
