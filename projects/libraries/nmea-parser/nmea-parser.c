#include "nmea-parser.h"
#include <string.h>

// Helper: convert ASCII hex digit to value
static uint8_t hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// Helper: parse fixed-point coordinate (DDMM.MMMM format)
static int32_t parse_coordinate(const char *str, char hemisphere) {
    int32_t degrees = 0;
    int32_t minutes = 0;
    const char *dot;
    int32_t result;
    
    if (!str || *str == '\0') return 0;
    
    // Find decimal point
    dot = strchr(str, '.');
    if (!dot) return 0;
    
    // Parse degrees (first 2 or 3 digits for lat/lon)
    if (hemisphere == 'N' || hemisphere == 'S') {
        // Latitude: DDMM.MMMM
        degrees = (str[0] - '0') * 10 + (str[1] - '0');
        str += 2;
    } else {
        // Longitude: DDDMM.MMMM
        degrees = (str[0] - '0') * 100 + (str[1] - '0') * 10 + (str[2] - '0');
        str += 3;
    }
    
    // Parse minutes MM.MMMM
    minutes = (str[0] - '0') * 10 + (str[1] - '0');
    str += 2;
    
    // Skip dot
    if (*str == '.') str++;
    
    // Parse fractional minutes (up to 4 digits)
    minutes *= 10000;
    while (*str >= '0' && *str <= '9') {
        minutes += (*str - '0') * 1000;
        str++;
        if (*str >= '0' && *str <= '9') minutes += (*str - '0') * 100;
        str++;
        if (*str >= '0' && *str <= '9') minutes += (*str - '0') * 10;
        str++;
        if (*str >= '0' && *str <= '9') minutes += (*str - '0');
        break;
    }
    
    // Convert to microdegrees: degrees + (minutes/60)
    result = degrees * 1000000L + (minutes * 1000000L / 60 / 10000);
    
    // Apply hemisphere
    if (hemisphere == 'S' || hemisphere == 'W') {
        result = -result;
    }
    
    return result;
}

// Helper: extract field from comma-separated sentence
static const char* get_field(const char *sentence, uint8_t field_num) {
    static char field_buffer[20];
    uint8_t current_field = 0;
    uint8_t i = 0;
    
    while (*sentence && current_field < field_num) {
        if (*sentence == ',') current_field++;
        sentence++;
    }
    
    if (current_field != field_num) return NULL;
    
    // Copy field to buffer
    while (*sentence && *sentence != ',' && i < 19) {
        field_buffer[i++] = *sentence++;
    }
    field_buffer[i] = '\0';
    
    return field_buffer;
}

void nmea_init(NMEAParser *parser) {
    parser->index = 0;
    parser->checksum = 0;
    parser->calculated_checksum = 0;
    parser->state = NMEA_IDLE;
}

uint8_t nmea_parse_char(NMEAParser *parser, char c) {
    switch (parser->state) {
        case NMEA_IDLE:
            if (c == '$') {
                parser->index = 0;
                parser->calculated_checksum = 0;
                parser->state = NMEA_READING;
                parser->buffer[parser->index++] = c;
            }
            break;
            
        case NMEA_READING:
            if (c == '*') {
                parser->buffer[parser->index++] = c;
                parser->state = NMEA_COMPLETE;
            } else if (c == '\r' || c == '\n') {
                parser->state = NMEA_ERROR;
            } else if (parser->index < 81) {
                parser->buffer[parser->index++] = c;
                parser->calculated_checksum ^= c;
            } else {
                parser->state = NMEA_ERROR;
            }
            break;
            
        case NMEA_COMPLETE:
            // Read checksum (2 hex digits)
            if (parser->index == 81 || parser->buffer[parser->index - 1] == '*') {
                parser->buffer[parser->index++] = c;
                if (parser->index >= 2 && parser->buffer[parser->index - 3] == '*') {
                    parser->buffer[parser->index] = '\0';
                    // Extract and verify checksum
                    parser->checksum = (hex_to_byte(parser->buffer[parser->index - 2]) << 4) |
                                      hex_to_byte(parser->buffer[parser->index - 1]);
                    
                    if (parser->checksum == parser->calculated_checksum) {
                        parser->state = NMEA_IDLE;
                        return 1; // Sentence complete and valid
                    } else {
                        parser->state = NMEA_ERROR;
                    }
                }
            }
            break;
            
        case NMEA_ERROR:
            if (c == '$') {
                parser->index = 0;
                parser->calculated_checksum = 0;
                parser->state = NMEA_READING;
                parser->buffer[parser->index++] = c;
            }
            break;
    }
    
    return 0;
}

uint8_t nmea_parse_gprmc(const char *sentence, GPSData *data) {
    const char *field;
    
    // Check if it's a GPRMC sentence
    if (strncmp(sentence, "$GPRMC", 6) != 0) return 0;
    
    // Field 1: Time (HHMMSS.sss)
    field = get_field(sentence, 1);
    if (field && strlen(field) >= 6) {
        data->hour = (field[0] - '0') * 10 + (field[1] - '0');
        data->minute = (field[2] - '0') * 10 + (field[3] - '0');
        data->second = (field[4] - '0') * 10 + (field[5] - '0');
    }
    
    // Field 2: Status (A=valid, V=invalid)
    field = get_field(sentence, 2);
    data->valid = (field && field[0] == 'A') ? 1 : 0;
    
    // Field 3: Latitude
    field = get_field(sentence, 3);
    if (field) {
        const char *hemisphere = get_field(sentence, 4);
        data->latitude = parse_coordinate(field, hemisphere ? hemisphere[0] : 'N');
    }
    
    // Field 5: Longitude
    field = get_field(sentence, 5);
    if (field) {
        const char *hemisphere = get_field(sentence, 6);
        data->longitude = parse_coordinate(field, hemisphere ? hemisphere[0] : 'E');
    }
    
    // Field 7: Speed (knots)
    field = get_field(sentence, 7);
    if (field && *field) {
        data->speed = 0;
        while (*field >= '0' && *field <= '9') {
            data->speed = data->speed * 10 + (*field - '0');
            field++;
        }
        if (*field == '.') {
            field++;
            if (*field >= '0' && *field <= '9') {
                data->speed = data->speed * 10 + (*field - '0');
            }
        }
    }
    
    // Field 8: Course (degrees)
    field = get_field(sentence, 8);
    if (field && *field) {
        data->course = 0;
        while (*field >= '0' && *field <= '9') {
            data->course = data->course * 10 + (*field - '0');
            field++;
        }
        if (*field == '.') {
            field++;
            if (*field >= '0' && *field <= '9') {
                data->course = data->course * 10 + (*field - '0');
            }
        }
    }
    
    // Field 9: Date (DDMMYY)
    field = get_field(sentence, 9);
    if (field && strlen(field) >= 6) {
        data->day = (field[0] - '0') * 10 + (field[1] - '0');
        data->month = (field[2] - '0') * 10 + (field[3] - '0');
        data->year = (field[4] - '0') * 10 + (field[5] - '0');
    }
    
    return 1;
}

uint8_t nmea_parse_gpgga(const char *sentence, GPSData *data) {
    // Similar structure for GPGGA if needed
    // For now, just time and position (no speed/course)
    return 0;
}
