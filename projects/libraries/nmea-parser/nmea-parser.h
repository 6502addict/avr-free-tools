#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <stdint.h>

// GPS data structure
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    int32_t latitude;    // 1/1000000 degree (microdegrees)
    int32_t longitude;   // 1/1000000 degree (microdegrees)
    uint16_t speed;      // 1/10 knots
    uint16_t course;     // 1/10 degrees
    uint8_t day;
    uint8_t month;
    uint8_t year;        // Years since 2000
    uint8_t valid;       // 1 = valid fix, 0 = invalid
} GPSData;

// Parser state machine
typedef enum {
    NMEA_IDLE,
    NMEA_READING,
    NMEA_COMPLETE,
    NMEA_ERROR
} NMEAState;

typedef struct {
    char buffer[82];     // Max NMEA sentence = 82 chars
    uint8_t index;
    uint8_t checksum;
    uint8_t calculated_checksum;
    NMEAState state;
} NMEAParser;

// Initialize parser
void nmea_init(NMEAParser *parser);

// Feed one character to parser (returns 1 when sentence complete)
uint8_t nmea_parse_char(NMEAParser *parser, char c);

// Parse completed GPRMC sentence into GPSData
uint8_t nmea_parse_gprmc(const char *sentence, GPSData *data);

// Parse completed GPGGA sentence into GPSData
uint8_t nmea_parse_gpgga(const char *sentence, GPSData *data);

#endif
