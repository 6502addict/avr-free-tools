#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

// Check if we're on a mega (with hardware TWI/I2C)
#if defined(__AVR_ATmega328P__)  || defined(__AVR_ATmega328__)  || \
    defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega1284__) || \
    defined(__AVR_ATmega168__)   || defined(__AVR_ATmega88__)   || \
    defined(__AVR_ATmega2560__)  || defined(__AVR_ATmega48__)
  #define HARDWARE_I2C
#endif

#ifdef HARDWARE_I2C

// Hardware I2C for ATmega

void i2c_init(void) {
    // Set SCL to 100kHz (assuming F_CPU = 16MHz)
    TWBR = ((F_CPU / 100000UL) - 16) / 2;
    TWSR = 0; // Prescaler = 1
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return (TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40;
}

uint8_t i2c_read(uint8_t ack) {
    if (ack) {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    } else {
        TWCR = (1 << TWINT) | (1 << TWEN);
    }
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

#else

// Software I2C for ATtiny (bit-banging)

#define I2C_SDA_HIGH() (DDRB &= ~(1 << I2C_SDA_PIN))
#define I2C_SDA_LOW()  (DDRB |= (1 << I2C_SDA_PIN))
#define I2C_SCL_HIGH() (DDRB &= ~(1 << I2C_SCL_PIN))
#define I2C_SCL_LOW()  (DDRB |= (1 << I2C_SCL_PIN))
#define I2C_SDA_READ() (PINB & (1 << I2C_SDA_PIN))

#define I2C_DELAY() _delay_us(5)

void i2c_init(void) {
    PORTB &= ~((1 << I2C_SDA_PIN) | (1 << I2C_SCL_PIN)); // Pull-ups off, output low
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
}

void i2c_start(void) {
    I2C_SDA_HIGH();
    I2C_SCL_HIGH();
    I2C_DELAY();
    I2C_SDA_LOW();
    I2C_DELAY();
    I2C_SCL_LOW();
}

void i2c_stop(void) {
    I2C_SDA_LOW();
    I2C_DELAY();
    I2C_SCL_HIGH();
    I2C_DELAY();
    I2C_SDA_HIGH();
    I2C_DELAY();
}

uint8_t i2c_write(uint8_t data) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            I2C_SDA_HIGH();
        } else {
            I2C_SDA_LOW();
        }
        I2C_DELAY();
        I2C_SCL_HIGH();
        I2C_DELAY();
        I2C_SCL_LOW();
        data <<= 1;
    }
    
    // Read ACK
    I2C_SDA_HIGH();
    I2C_DELAY();
    I2C_SCL_HIGH();
    I2C_DELAY();
    uint8_t ack = !I2C_SDA_READ();
    I2C_SCL_LOW();
    
    return ack;
}

uint8_t i2c_read(uint8_t ack) {
    uint8_t data = 0;
    uint8_t i;
    
    I2C_SDA_HIGH();
    for (i = 0; i < 8; i++) {
        I2C_DELAY();
        I2C_SCL_HIGH();
        I2C_DELAY();
        data = (data << 1) | (I2C_SDA_READ() ? 1 : 0);
        I2C_SCL_LOW();
    }
    
    // Send ACK/NACK
    if (ack) {
        I2C_SDA_LOW();
    } else {
        I2C_SDA_HIGH();
    }
    I2C_DELAY();
    I2C_SCL_HIGH();
    I2C_DELAY();
    I2C_SCL_LOW();
    I2C_SDA_HIGH();
    
    return data;
}

#endif

