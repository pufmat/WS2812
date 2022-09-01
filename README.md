# Pufferfish's WS2812

Fast and light weight header-only library written in C for WS2812 based LEDs. Only supports 8MHz AVR microcontrollers.

## Example

Rainbow Chase:
```C
#include "ws2812.h"
#include <util/delay.h>

#define LED_COUNT    30
#define LED_PORT     PORTB
#define LED_DDR      DDRB
#define LED_MASK     _BV(1)

PUF_WS2812(write_ws2812, LED_PORT)

uint8_t data[LED_COUNT * 3];

int main(){
    LED_DDR |= LED_MASK;
    
    while(1){
        for(int i = 0; i < 18; i += 3){
            for(int j = 0; j < LED_COUNT * 3; j++){
                data[j] = ((i + j) * 10 + 4) / 18 % 2 * 255;
            }
            
            write_ws2812(data, LED_COUNT, LED_MASK);
            _delay_ms(250);
        }
    }
}
```