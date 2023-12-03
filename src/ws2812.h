#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

#define PUF_WS2812(NAME, PORT)                                             \
    void NAME(uint8_t *data, size_t len, uint8_t mask){                    \
        const uint8_t  high = PORT |  mask;                                \
        const uint8_t  low  = PORT & ~mask;                                \
        const uint8_t *end  = data + len * 3;                              \
                                                                           \
        while(data != end){                                                \
            volatile uint8_t byte = *data++;                               \
            volatile uint8_t iter = 1;                                     \
                                                                           \
            asm volatile(                                                  \
                "_loop%=:"             "\n\t" /* loop:            +0    */ \
                "out %[port], %[high]" "\n\t" /* port = high      +1    */ \
                "lsl %[iter]"          "\n\t" /* iter <<= 1 --> C +1    */ \
                "sbrs %[byte], 7"      "\n\t" /* if(byte & 128)   +1/+2 */ \
                "out %[port], %[low]"  "\n\t" /*     port = low   +1    */ \
                "breq _break%="        "\n\t" /* if(C) goto break +1/+2 */ \
                "lsl %[byte]"          "\n\t" /* byte <<= 1 --> C +1    */ \
                "out %[port], %[low]"  "\n\t" /* port = low       +1    */ \
                "rjmp _loop%="         "\n\t" /* goto loop        +2    */ \
                "_break%=:"            "\n\t" /* break:           +0    */ \
                "out %[port], %[low]"  "\n\t" /* port = low       +1    */ \
                :                                                          \
                [byte] "+r" (byte),                                        \
                [iter] "+r" (iter)                                         \
                :                                                          \
                [port] "I"  (_SFR_IO_ADDR(PORT)),                          \
                [high] "r"  (high),                                        \
                [low]  "r"  (low),                                         \
                [end]  "e"  (end)                                          \
            );                                                             \
        }                                                                  \
    }
