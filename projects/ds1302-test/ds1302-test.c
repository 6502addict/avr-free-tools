#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <ds1302.h>
#include "uart_console.h"

#define DS1302_ON  0
#define DS1302_OFF 1


// Parse date string YYYY/MM/DD
static int parse_date(char *str, int *year, int *month, int *date) {
    if (sscanf(str, "%d/%d/%d", year, month, date) != 3) 
        return 0;
    if (*year < 2000 || *year > 2099 || *month < 1 || *month > 12 || *date < 1 || *date > 31) 
        return 0;
    *year = *year - 2000;
    return 1;
}

// Parse time string HH:MM:SS
static int parse_time(char *str, int *hours, int *minutes, int *seconds) {
    if (sscanf(str, "%d:%d:%d", hours, minutes, seconds) != 3) 
        return 0;
    if (*hours < 0 || *hours > 23 || *minutes < 0 || *minutes > 59 || *seconds < 0 || *seconds > 59) 
        return 0;
    return 1;
}

// Calculate day of week (0=Sun, 1=Mon, ..., 6=Sat) - returns 1-7 for DS1302
static inline uint8_t day_of_week(int year, int month, int date) {
    // Zeller's congruence
    if (month < 3) {
        month += 12;
        year--;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (date + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
    return ((h + 6) % 7) + 1;  // Convert to 1-7 with 1=Sunday
}

// day name
static inline char *day_name(uint8_t day) {
 char *days[] = {"", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  if (day >= 1 && day <= 7) 
    return days[day];
  return NULL;
}

// Convert decimal to BCD
static inline uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

// Convert BCD to decimal
static uint8_t bcd_to_dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

// Set date
void ds1302_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day) {
  ds1302_write_rtc(DS1302_YEAR,  dec_to_bcd(year));
  ds1302_write_rtc(DS1302_MONTH, dec_to_bcd(month));
  ds1302_write_rtc(DS1302_DAY,   dec_to_bcd(day));
  ds1302_write_rtc(DS1302_DATE,  dec_to_bcd(date));
}

// Get date
void ds1302_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *day) {
  *year  = bcd_to_dec(ds1302_read_rtc(DS1302_YEAR));
  *month = bcd_to_dec(ds1302_read_rtc(DS1302_MONTH));
  *date  = bcd_to_dec(ds1302_read_rtc(DS1302_DATE));
  *day   = bcd_to_dec(ds1302_read_rtc(DS1302_DAY));
}

// Set time
void ds1302_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  ds1302_write_rtc(DS1302_SEC,  dec_to_bcd(seconds));
  ds1302_write_rtc(DS1302_MIN,  dec_to_bcd(minutes));
  ds1302_write_rtc(DS1302_HOUR, dec_to_bcd(hours));
}

// Get time
void ds1302_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
  *seconds = bcd_to_dec(ds1302_read_rtc(DS1302_SEC) & 0x7F);
  *minutes = bcd_to_dec(ds1302_read_rtc(DS1302_MIN));
  *hours   = bcd_to_dec(ds1302_read_rtc(DS1302_HOUR));
}

// Halt/resume clock
void ds1302_run(uint8_t state) {
  uint8_t sec = ds1302_read_rtc(DS1302_SEC);
  switch(state) {
  case DS1302_ON:
    ds1302_write_rtc(DS1302_SEC, sec & 0x7F);
    break;
  case DS1302_OFF:
    ds1302_write_rtc(DS1302_SEC, sec | 0x80);
    break;
  }
}

// Protect/unprotect
void ds1302_protect(uint8_t state) {
  switch(state) {
  case DS1302_ON:
    ds1302_write_rtc(DS1302_WP, 0x80);  // Enable write protect
    break;
  case DS1302_OFF:
    ds1302_write_rtc(DS1302_WP, 0x00);   // Disable write protect
    break;
  }
}

// Halt/resume clock
static bool ds1302_protected(void) {
  return (ds1302_read_rtc(DS1302_WP) & 0x80) ? true : false;
}

// Process commands
static void process_command(char *cmd) {
  char *arg;
  
  // Skip leading spaces
  while (*cmd == ' ') cmd++;
  
  // Find argument (after space)
  arg = strchr(cmd, ' ');
  if (arg) {
    *arg = '\0';
    arg++;
    while (*arg == ' ') arg++;  // Skip spaces
  }
  
  // Convert command to lowercase
  for (char *p = cmd; *p; p++) {
    *p = tolower(*p);
  }

  if (strcmp(cmd, "write") == 0 && arg) {
    char type[10];
    uint8_t addr, value, nvalue;
    
    if (!ds1302_protected()) {
      if (sscanf(arg, "%s %hhx %hhx", type, &addr, &value) == 3) {
	if (strcmp(type, "ram") == 0) {
	  ds1302_write_ram(addr, value);
	  nvalue = ds1302_read_ram(addr);
	  printf("Write RAM[0x%02X] = 0x%02X %02X\r\n", addr, value, nvalue);
	} else if (strcmp(type, "rtc") == 0) {
	  ds1302_write_rtc(addr, value);
	  printf("Write RTC[0x%02X] = 0x%02X\r\n", addr, value);
	} else {
	  printf("Error: Use 'ram' or 'rtc'. Example: write ram 00 AA\r\n");
	}
      } else {
	printf("Error: write ram XX YY or write rtc XX YY\r\n");
      }
    } else {
      printf("d1302 is write protected\r\n");
    }
  } else if (strcmp(cmd, "read") == 0 && arg) {
    char type[10];
    uint8_t addr;
    
    if (sscanf(arg, "%s %hhx", type, &addr) == 2) {
      if (strcmp(type, "ram") == 0) {
	uint8_t value = ds1302_read_ram(addr);
	printf("Read RAM[0x%02X]: 0x%02X\r\n", addr, value);
      } else if (strcmp(type, "rtc") == 0) {
	uint8_t value = ds1302_read_rtc(addr);
	printf("Read RTC[0x%02X]: 0x%02X\r\n", addr, value);
      } else {
	printf("Error: Use 'ram' or 'rtc'. Example: read ram 00\r\n");
      }
    } else {
      printf("Error: read ram XX or read rtc XX\r\n");
    }

  } else if (strcmp(cmd, "protect") == 0 && arg) {
    char onoff[10];

    if (sscanf(arg, "%s", onoff) == 1) {
      if (strcmp(onoff, "on") == 0) {
	ds1302_protect(DS1302_ON);
	printf("ds1302 write protected\n");
      } else if (strcmp(onoff, "off") == 0) {
	ds1302_protect(DS1302_OFF);
	printf("ds1302 not write protected\n");
      } else {
	printf("Error: Use 'on' or 'off'. Example: protect on\r\n");
      }
    }
    
  } else if (strcmp(cmd, "set") == 0 && arg) {
    // Check if next word is "date" or "time"
    char *subcmd = arg;
    char *value = strchr(arg, ' ');
    if (!ds1302_protected()) {
      if (value) {
	*value = '\0';
	value++;
	while (*value == ' ') value++;
      }
      for (char *p = subcmd; *p; p++) 
	*p = tolower(*p);
      if (strcmp(subcmd, "date") == 0 && value) {
	int  year, month, date, day;
	
	if (parse_date(value, &year, &month, &date)) {
	  day = day_of_week(2000 + year, month, date);
	  ds1302_set_date((uint8_t) year, (uint8_t) month, (uint8_t) date, (uint8_t) day);
	  printf("Date set to %04d/%02d/%02d (%s)\r\n", 2000 + year, month, date, day_name(day));
	} else {
	  printf("Error: Invalid date format. Use YYYY/MM/DD\r\n");
	}
      } else {
	printf("d1302 is write protected\r\n");
      }
    } else if (strcmp(subcmd, "time") == 0 && value) {
      int  hours, minutes, seconds;
      
      if (parse_time(value, &hours, &minutes, &seconds)) {
	ds1302_set_time((uint8_t) hours, (uint8_t) minutes, (uint8_t) seconds);
	printf("Time set to %02d:%02d:%02d\r\n", hours, minutes, seconds);
      } else {
	printf("Error: Invalid time format. Use HH:MM:SS\r\n");
      }
    } else {
      printf("Error: Unknown set command. Use 'set date YYYY/MM/DD' or 'set time HH:MM:SS'\r\n");
    }
  } else if (strcmp(cmd, "get") == 0 && arg) {
    for (char *p = arg; *p; p++) {
      *p = tolower(*p);
    }
    if (strcmp(arg, "date") == 0) {
      uint8_t year, month, date, day;

      ds1302_get_date(&year, &month, &date, &day);
      printf("%04d/%02d/%02d\r\n", 2000 + year, month, date);
    } else if (strcmp(arg, "time") == 0) {
      uint8_t  hours, minutes, seconds;

      ds1302_get_time(&hours, &minutes, &seconds);
      printf("%02d:%02d:%02d\r\n", hours, minutes, seconds);
    } else {
      printf("Error: Unknown get command. Use 'get date', 'get time', or 'get dt'\r\n");
    }
  } else if (strcmp(cmd, "stop") == 0) {
    ds1302_run(DS1302_OFF);
    printf("Clock halted\r\n");
  } else if (strcmp(cmd, "start") == 0) {
    ds1302_run(DS1302_ON);
    printf("Clock resumed\r\n");
  } else if (strcmp(cmd, "status") == 0) {
    uint8_t year, month, date, day, hours, minutes, seconds;
    uint8_t sec = ds1302_read_rtc(DS1302_SEC);

    ds1302_get_date(&year, &month, &date, &day);
    ds1302_get_time(&hours, &minutes, &seconds);
    printf("Clock is %s\r\n", (sec & 0x80) ? "halted" : "running");
    printf("Current: %04d/%02d/%02d %02d:%02d:%02d (%s)\r\n", 
	   2000 + year, month, date,
	   hours, minutes, seconds, day_name(day));
  } else if (strcmp(cmd, "help") == 0) {
    printf("DS1302 RTC Test Commands:\r\n");
    printf("  set date YYYY/MM/DD  - Set date\r\n");
    printf("  set time HH:MM:SS    - Set time\r\n");
    printf("  get date             - Get date\r\n");
    printf("  get time             - Get time\r\n");
    printf("  stop                 - Stop the clock\r\n");
    printf("  start                - Start the clock\r\n");
    printf("  status               - Show clock status\r\n");
    printf("  help                 - Show this help\r\n");
  } else {
    printf("Error: Unknown command. Type 'help' for commands.\r\n");
  }
}

int main(void) {
  char buffer[64];
  uint8_t buf_idx = 0;
  
  console_init();
  ds1302_init();
  
  printf("\r\nDS1302 RTC Test v1.2\r\n");
  printf("Type 'help' for commands\r\n\r\n");
  printf("> ");
  ds1302_write_rtc(DS1302_WP, 0x00);  
  while (1) {
    if (uart_available()) {
      char c = getchar();
      
      if (c == '\r' || c == '\n') {
	if (buf_idx > 0) {
	  printf("\r\n");
	  buffer[buf_idx] = '\0';
	  process_command(buffer);
	  buf_idx = 0;
	  printf("> ");
	}
      } else if (c == '\b' || c == 127) {  // Backspace
	if (buf_idx > 0) {
	  buf_idx--;
	  printf("\b \b");
	}
      } else if (buf_idx < sizeof(buffer) - 1) {
	buffer[buf_idx++] = c;
	putchar(c);  // Echo
      }
    }
  }
  
  return 0;
}
