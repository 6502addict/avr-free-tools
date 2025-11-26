#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <wheel.h>
#include <util/delay.h>


static Wheel wheel;
static volatile uint8_t button_pressed = 0;


static void wheel_event(uint8_t state) {
  static int rot, lrot = -1;
  
  if (state == WHEEL_BUTTON) {
    wheel_button((int)wheel.rotation);
  } else {
    if (state == WHEEL_FORWARD) {
      if (wheel.rotation + wheel.step <= wheel.maxi)
        wheel.rotation += wheel.step;
      else
        wheel.rotation = wheel.maxi;  // Clamp to max
    }
    if (state == WHEEL_BACKWARD) {
      if (wheel.rotation - wheel.step >= wheel.mini)
        wheel.rotation -= wheel.step;
      else
        wheel.rotation = wheel.mini;  // Clamp to min
    }
    
    rot = wheel.rotation;
    if (rot == lrot)
      return;
    wheel_position(rot);
    lrot = rot;
  }
}

#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny25__) || \
    defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)

// ATtiny - Pin Change Interrupt with edge detection
static volatile uint8_t last_clk_state = 1;

void wheel_init(int mini, int maxi, uint8_t step) {
  memset(&wheel, 0, sizeof(Wheel));
  wheel.mini = mini;
  wheel.maxi = maxi;
  wheel.step = step;
  wheel.rotation = 0;
  
  // Set pins as inputs with pull-ups
  WHEEL_CLK_DDR &= ~(1 << WHEEL_CLK_BIT);
  WHEEL_DT_DDR &= ~(1 << WHEEL_DT_BIT);
  WHEEL_SW_DDR &= ~(1 << WHEEL_SW_BIT);
  
  WHEEL_CLK_PORT |= (1 << WHEEL_CLK_BIT);
  WHEEL_DT_PORT |= (1 << WHEEL_DT_BIT);
  WHEEL_SW_PORT |= (1 << WHEEL_SW_BIT);
  
  // Enable pin change interrupt
  GIMSK |= (1 << PCIE);
  PCMSK |= (1 << WHEEL_CLK_BIT) | (1 << WHEEL_DT_BIT) | (1 << WHEEL_SW_BIT);
  
  // Read initial CLK state
  last_clk_state = (WHEEL_CLK_PIN & (1 << WHEEL_CLK_BIT)) ? 1 : 0;
  
  sei();
}

ISR(PCINT0_vect) {
  uint8_t current_clk = (WHEEL_CLK_PIN & (1 << WHEEL_CLK_BIT)) ? 1 : 0;
  
  // Only process on falling edge of CLK
  if (last_clk_state == 1 && current_clk == 0) {
    _delay_us(100);  // Debounce
    
    // Re-read after delay
    uint8_t dt = (WHEEL_DT_PIN & (1 << WHEEL_DT_BIT)) ? 1 : 0;
    
    if (dt) {
      wheel_event(WHEEL_FORWARD);
    } else {
      wheel_event(WHEEL_BACKWARD);
    }
  }
  
  last_clk_state = current_clk;
}

#else

// ATmega - External Interrupt INT0
void wheel_init(int mini, int maxi, uint8_t step) {
  memset(&wheel, 0, sizeof(Wheel));
  wheel.mini = mini;
  wheel.maxi = maxi;
  wheel.step = step;
  wheel.rotation = 0;
  
  // Set pins as inputs with pull-ups
  WHEEL_CLK_DDR &= ~(1 << WHEEL_CLK_BIT);
  WHEEL_DT_DDR &= ~(1 << WHEEL_DT_BIT);
  WHEEL_SW_DDR &= ~(1 << WHEEL_SW_BIT);
  
  WHEEL_CLK_PORT |= (1 << WHEEL_CLK_BIT);
  WHEEL_DT_PORT |= (1 << WHEEL_DT_BIT);
  WHEEL_SW_PORT |= (1 << WHEEL_SW_BIT);
  
  // Configure INT0 to trigger on FALLING edge
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);
  
  // Enable INT0
  EIMSK |= (1 << INT0);
  
  sei();
}

/*
ISR(INT0_vect) {
  static uint8_t last_clk = 0;
  static uint8_t stable_count = 0;
  
  uint8_t current_clk = (WHEEL_CLK_PIN & (1 << WHEEL_CLK_BIT)) ? 1 : 0;
  
  // Only process if signal has been stable
  if (current_clk == last_clk) {
    stable_count++;
    if (stable_count > 2) {  // Require 3 consistent reads
      // Read DT when CLK is stable low
      if (current_clk == 0) {
        if (WHEEL_DT_PIN & (1 << WHEEL_DT_BIT)) {
          wheel_event(WHEEL_FORWARD);
        } else {
          wheel_event(WHEEL_BACKWARD);
        }
      }
      stable_count = 0;
    }
  } else {
    stable_count = 0;
  }
  
  last_clk = current_clk;
}
*/

ISR(INT0_vect) {
  // Small delay to let bounce settle
  _delay_us(100);  // 100 microseconds
  
  // Re-read pins after delay
  uint8_t clk = (WHEEL_CLK_PIN & (1 << WHEEL_CLK_BIT)) ? 1 : 0;
  uint8_t dt = (WHEEL_DT_PIN & (1 << WHEEL_DT_BIT)) ? 1 : 0;
  
  // Only process if CLK is still low (valid falling edge)
  if (clk == 0) {
    if (dt) {
      wheel_event(WHEEL_FORWARD);
    } else {
      wheel_event(WHEEL_BACKWARD);
    }
  }
}

/*
ISR(INT0_vect) {
  // Read DT when CLK falls
  if (WHEEL_DT_PIN & (1 << WHEEL_DT_BIT)) {
    wheel_event(WHEEL_BACKWARD);
  } else {
    wheel_event(WHEEL_FORWARD);
  }
}
*/

#endif

void wheel_reset(int mini, int maxi) {
  wheel.rotation = 0;
  wheel.mini = mini;
  wheel.maxi = maxi;
}

int wheel_get_position(void) {
  return (int)wheel.rotation;
}
