#include "ws2812.h"
#include <util/delay.h>

#define LED_COUNT    30
#define LED_PORT     PORTB
#define LED_DDR      DDRB
#define LED_PIN      1

PUF_WS2812(write_ws2812, LED_PORT)

uint8_t data[LED_COUNT * 3];

int main(){
    LED_DDR |= (1 << LED_PIN);
    
    while(1){
        for(int i = 0; i < 18; i += 3){
            for(int j = 0; j < LED_COUNT * 3; j++){
                data[j] = ((i + j) * 10 + 4) / 18 % 2 * 255;
            }
            write_ws2812(data, LED_COUNT, LED_PIN);
            _delay_ms(250);
        }
    }
}