#ifndef I2C_H
#define I2C_H

// For ATtiny: define software I2C pins before including this header
// Example: #define I2C_SDA_PIN PB0
//          #define I2C_SCL_PIN PB2

#ifdef __AVR_ATtiny13__
  #ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN PB0
  #endif
  #ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN PB2
  #endif
#endif

#ifdef __AVR_ATtiny25__
  #ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN PB0
  #endif
  #ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN PB2
  #endif
#endif

#ifdef __AVR_ATtiny45__
  #ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN PB0
  #endif
  #ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN PB2
  #endif
#endif

#ifdef __AVR_ATtiny85__
  #ifndef I2C_SDA_PIN
    #define I2C_SDA_PIN PB0
  #endif
  #ifndef I2C_SCL_PIN
    #define I2C_SCL_PIN PB2
  #endif
#endif

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);

#endif // I2C_H
