#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "uart_console.h"

#define F_CPU 16000000UL

// Pin definitions (Arduino pin 9 = PB1 = OC1A)
#define OUTPUT_PIN PB1
#define OUTPUT_DDR DDRB

// Command buffer
#define CMD_BUFFER_SIZE 64
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;

// Current frequency state
static uint16_t current_frequency = 0;
static bool generator_running = false;

// Initialize frequency generator using Timer1
void init_frequency_generator(uint16_t frequency) {
    if (frequency == 0) return;
    
    // Set pin as output
    OUTPUT_DDR |= (1 << OUTPUT_PIN);
    
    // Calculate compare value for different prescalers
    uint32_t ocr_value;
    uint8_t prescaler_bits;
    
    if (frequency >= 245) {
        // Prescaler = 1
        ocr_value = (F_CPU / (2UL * frequency)) - 1;
        prescaler_bits = (1 << CS10);
    } else if (frequency >= 31) {
        // Prescaler = 8
        ocr_value = (F_CPU / (16UL * frequency)) - 1;
        prescaler_bits = (1 << CS11);
    } else if (frequency >= 4) {
        // Prescaler = 64
        ocr_value = (F_CPU / (128UL * frequency)) - 1;
        prescaler_bits = (1 << CS11) | (1 << CS10);
    } else {
        // Prescaler = 256
        ocr_value = (F_CPU / (512UL * frequency)) - 1;
        prescaler_bits = (1 << CS12);
    }
    
    // Limit to 16-bit max
    if (ocr_value > 65535) ocr_value = 65535;
    
    // Configure Timer1 in CTC mode with toggle on compare match
    TCCR1A = (1 << COM1A0);  // Toggle OC1A on compare match
    TCCR1B = (1 << WGM12) | prescaler_bits;  // CTC mode + prescaler
    OCR1A = ocr_value;
    TCNT1 = 0;
    
    current_frequency = frequency;
    generator_running = true;
}

// Stop frequency generation
void stop_frequency_generator(void) {
    TCCR1B = 0;  // Stop timer
    TCCR1A = 0;
    OUTPUT_DDR &= ~(1 << OUTPUT_PIN);  // Set as input to ensure low
    current_frequency = 0;
    generator_running = false;
}

// Parse frequency value from string (e.g., "1200hz" or "1200")
uint16_t parse_frequency(const char *str) {
    uint16_t freq = 0;
    
    // Skip whitespace
    while (*str && isspace(*str)) str++;
    
    // Parse digits
    while (*str && isdigit(*str)) {
        freq = freq * 10 + (*str - '0');
        str++;
    }
    
    return freq;
}

// Process command
void process_command(const char *cmd) {
    char cmd_copy[CMD_BUFFER_SIZE];
    strncpy(cmd_copy, cmd, CMD_BUFFER_SIZE - 1);
    cmd_copy[CMD_BUFFER_SIZE - 1] = '\0';
    
    // Convert to lowercase for comparison
    for (char *p = cmd_copy; *p; p++) {
        *p = tolower(*p);
    }
    
    // Remove leading/trailing whitespace
    char *start = cmd_copy;
    while (*start && isspace(*start)) start++;
    
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) *end-- = '\0';
    
    if (strlen(start) == 0) return;
    
    // Check for "stop" command
    if (strncmp(start, "stop", 4) == 0) {
        stop_frequency_generator();
        printf("Frequency generator stopped\n");
        return;
    }
    
    // Check for "frequency" command
    if (strncmp(start, "frequency", 9) == 0) {
        char *freq_str = start + 9;
        uint16_t freq = parse_frequency(freq_str);
        
        if (freq > 0 && freq <= 8000000) {
            stop_frequency_generator();
            init_frequency_generator(freq);
            printf("Generating %u Hz on pin 9\n", freq);
        } else {
            printf("Invalid frequency: %u Hz (range: 1-8000000)\n", freq);
        }
        return;
    }
    
    // Check for "status" command
    if (strncmp(start, "status", 6) == 0) {
        if (generator_running) {
            printf("Generator running at %u Hz\n", current_frequency);
        } else {
            printf("Generator stopped\n");
        }
        return;
    }
    
    // Check for "help" command
    if (strncmp(start, "help", 4) == 0) {
        printf("\nAvailable commands:\n");
        printf("  frequency <value>  - Generate frequency (e.g., frequency 1200hz)\n");
        printf("  stop               - Stop frequency generation\n");
        printf("  status             - Show current status\n");
        printf("  help               - Show this help\n\n");
        return;
    }
    
    printf("Unknown command: %s (type 'help' for commands)\n", start);
}

// Read and process serial input
void handle_serial_input(void) {
    if (!uart_available()) return;
    
    char c = getchar();
    
    if (c == '\n' || c == '\r') {
        if (cmd_index > 0) {
            cmd_buffer[cmd_index] = '\0';
            printf("\n");
            process_command(cmd_buffer);
            cmd_index = 0;
            printf("> ");
            fflush(stdout);
        }
    } else if (c == 127 || c == 8) {
        // Backspace
        if (cmd_index > 0) {
            cmd_index--;
        }
    } else if (c >= 32 && c < 127) {
        // Printable character
        if (cmd_index < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_index++] = c;
        }
    }
}

int main(void) {
    // Initialize UART console
    console_init();
    
    // Small delay for terminal to connect
    _delay_ms(500);
    
    // Print welcome message
    printf("\n=== AVR Frequency Generator ===\n");
    printf("Output: Pin 9 (OC1A)\n");
    printf("Type 'help' for commands\n\n");
    printf("> ");
    fflush(stdout);
    
    // Main loop
    while (1) {
        handle_serial_input();
    }
    
    return 0;
}
