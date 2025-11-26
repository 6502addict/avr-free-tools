#ifndef BME280_H
#define BME280_H

// BME280 I2C address (default is 0x76, alternative is 0x77)
#ifndef BME280_ADDR
#define BME280_ADDR 0x77
#endif

// Return codes
#define BME280_OK    0
#define BME280_ERROR 1

// Initialize BME280
uint8_t bme280_init(void);

// Read measurements
int32_t bme280_read_temperature(void);  // Returns temperature in 0.01°C
uint32_t bme280_read_pressure(void);    // Returns pressure in Pa
uint32_t bme280_read_humidity(void);    // Returns humidity in 0.01%

#endif // BME280_H
