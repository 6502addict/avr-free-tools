; serial_echo_tiny.asm
; Software Serial Echo for ATtiny13/25/45/85
; Emulates UART using Timer0 and bit-banging
; 8MHz crystal, configurable baud rate

.IFDEF ATtiny13
.INCLUDE "tn13def.inc"
.ENDIF
.IFDEF ATtiny25
.INCLUDE "tn25def.inc"
.ENDIF
.IFDEF ATtiny45
.INCLUDE "tn45def.inc"
.ENDIF
.IFDEF ATtiny85
.INCLUDE "tn85def.inc"
.ENDIF

; Pin definitions for software UART
.equ TX_PIN = PB1       ; TX on PB1
.equ RX_PIN = PB0       ; RX on PB0

; Timer settings for baud rate generation
; For 9600 baud at 8MHz: bit time = 833 cycles
; Timer0 with prescaler /8 = 104 counts per bit
.equ BAUD_9600_TIMER = 104
.equ PRESCALER_BITS = (1<<CS01)  ; Prescaler /8

; For higher baud rates (adjust as needed):
; 19200 baud: 52 counts
; 38400 baud: 26 counts
.equ BIT_TIME = BAUD_9600_TIMER

; Registers usage
.def temp = r16
.def uart_data = r17
.def bit_cnt = r18
.def rx_data = r19

.org 0x0000
    rjmp main           ; Reset vector

main:
    ; Initialize stack pointer
    ; ATtiny13/25 have no SPH register, ATtiny45/85 do
    ldi temp, low(RAMEND)
    out SPL, temp
.IFDEF ATtiny45
    ldi temp, high(RAMEND)
    out SPH, temp
.ENDIF
.IFDEF ATtiny85
    ldi temp, high(RAMEND)
    out SPH, temp
.ENDIF
    
    ; Initialize I/O pins
    sbi DDRB, TX_PIN    ; TX as output
    sbi PORTB, TX_PIN   ; TX idle high
    cbi DDRB, RX_PIN    ; RX as input
    sbi PORTB, RX_PIN   ; Enable pull-up on RX
    
    ; Initialize Timer0 for bit timing
    ldi temp, BIT_TIME
    out OCR0A, temp     ; Set compare value
    
    ldi temp, (1<<WGM01) ; CTC mode
    out TCCR0A, temp
    
    ldi temp, PRESCALER_BITS
    out TCCR0B, temp    ; Start timer with prescaler
    
main_loop:
    rcall uart_getc     ; Receive byte in r24
    rcall uart_putc     ; Echo it back
    rjmp main_loop

; Software UART transmit
; Input: r24 = byte to transmit
uart_putc:
    push temp
    push uart_data
    push bit_cnt
    
    mov uart_data, r24
    ldi bit_cnt, 10    ; 1 start + 8 data + 1 stop
    
    ; Send start bit (low)
    cbi PORTB, TX_PIN
    rcall wait_bit_time
    
    ; Send 8 data bits
send_data_bits:
    sbrc uart_data, 0  ; Check LSB
    sbi PORTB, TX_PIN  ; Send 1
    sbrs uart_data, 0
    cbi PORTB, TX_PIN  ; Send 0
    
    lsr uart_data      ; Shift to next bit
    rcall wait_bit_time
    
    dec bit_cnt
    cpi bit_cnt, 2     ; Still have data bits?
    brne send_data_bits
    
    ; Send stop bit (high)
    sbi PORTB, TX_PIN
    rcall wait_bit_time
    
    pop bit_cnt
    pop uart_data
    pop temp
    ret

; Software UART receive
; Output: r24 = received byte
uart_getc:
    push temp
    push bit_cnt
    push rx_data
    
    ; Wait for start bit (low)
wait_start:
    sbic PINB, RX_PIN
    rjmp wait_start
    
    ; Wait half bit time to sample in middle
    rcall wait_half_bit_time
    
    ; Verify start bit
    sbic PINB, RX_PIN
    rjmp wait_start    ; False start, try again
    
    ; Receive 8 data bits
    ldi bit_cnt, 8
    clr rx_data
    
receive_bits:
    rcall wait_bit_time
    
    clc                ; Clear carry
    sbic PINB, RX_PIN
    sec                ; Set carry if bit is 1
    
    ror rx_data        ; Rotate carry into MSB
    
    dec bit_cnt
    brne receive_bits
    
    ; Wait for stop bit
    rcall wait_bit_time
    
    ; Return received byte in r24
    mov r24, rx_data
    
    pop rx_data
    pop bit_cnt
    pop temp
    ret

; Wait one bit time using Timer0
wait_bit_time:
    push temp
    
    ; Clear timer counter
    clr temp
    out TCNT0, temp
    
    ; Clear compare flag
.IFDEF ATtiny13
    ldi temp, (1<<OCF0A)
    out TIFR0, temp     ; ATtiny13 uses TIFR0
.ELSE
    ldi temp, (1<<OCF0A)
    out TIFR, temp      ; ATtiny25/45/85 use TIFR
.ENDIF
    
    ; Wait for compare match
wait_bit:
.IFDEF ATtiny13
    in temp, TIFR0
.ELSE
    in temp, TIFR
.ENDIF
    sbrs temp, OCF0A
    rjmp wait_bit
    
    pop temp
    ret

; Wait half bit time for start bit sampling
wait_half_bit_time:
    push temp
    
    ; Clear timer counter
    clr temp
    out TCNT0, temp
    
    ; Wait for half the bit time
wait_half:
    in temp, TCNT0
    cpi temp, (BIT_TIME/2)
    brlo wait_half
    
    pop temp
    ret
