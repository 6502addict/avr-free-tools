; bin_to_hex.S
; Assembly function to convert binary to hexadecimal ASCII
; Called from C: bin_to_hex(value, *hex_high, *hex_low)
; 
; C calling convention for AVR-GCC:
; - First parameter (value) in r24
; - Second parameter (hex_high pointer) in r22:r23
; - Third parameter (hex_low pointer) in r20:r21
; - Return value in r24:r25 (not used here)
; - Must preserve r2-r17, r28-r29

.global bin_to_hex

.section .text

bin_to_hex:
    ; Save registers we'll use
    push r18
    push r19
    push r26
    push r27
    push r30
    push r31
    
    ; r24 = value to convert
    ; r22:r23 = pointer to hex_high
    ; r20:r21 = pointer to hex_low
    
    ; Move pointers to X and Z registers
    movw r26, r22       ; X = hex_high pointer
    movw r30, r20       ; Z = hex_low pointer
    
    ; Process high nibble (upper 4 bits)
    mov r18, r24        ; Copy value
    swap r18            ; Swap nibbles
    andi r18, 0x0F      ; Mask lower nibble
    
    ; Convert high nibble to ASCII
    cpi r18, 10         ; Compare with 10
    brlo high_digit     ; Branch if less than 10 (0-9)
    
high_letter:
    ; A-F (value 10-15)
    subi r18, -55       ; Add 55 ('A' - 10 = 55)
    rjmp store_high
    
high_digit:
    ; 0-9 (value 0-9)
    subi r18, -48       ; Add 48 ('0' = 48)
    
store_high:
    st X, r18           ; Store high nibble ASCII at *hex_high
    
    ; Process low nibble (lower 4 bits)
    mov r19, r24        ; Copy value
    andi r19, 0x0F      ; Mask upper nibble
    
    ; Convert low nibble to ASCII
    cpi r19, 10         ; Compare with 10
    brlo low_digit      ; Branch if less than 10 (0-9)
    
low_letter:
    ; A-F (value 10-15)
    subi r19, -55       ; Add 55 ('A' - 10 = 55)
    rjmp store_low
    
low_digit:
    ; 0-9 (value 0-9)
    subi r19, -48       ; Add 48 ('0' = 48)
    
store_low:
    st Z, r19           ; Store low nibble ASCII at *hex_low
    
    ; Restore registers
    pop r31
    pop r30
    pop r27
    pop r26
    pop r19
    pop r18
    
    ret

