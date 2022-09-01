#include <avr/io.h>
#include <stdint.h>

#define PUF_WS2812__STRINGIZE(X) #X
#define PUF_WS2812_STRINGIZE(X) PUF_WS2812__STRINGIZE(X)

#define PUF_WS2812_LOAD_BYTE                                   \
    "ld %[tmp], %a[ptr]+"  "\n\t" /* tmp = *ptr++     +2    */

#define PUF_WS2812__WRITE_BIT(X)                               \
    "out %[port], %[high]" "\n\t" /* port = high      +1    */ \
    "rol %[tmp]"           "\n\t" /* tmp <<= 1 --> C  +1    */ \
    "brcs __L" X           "\n\t" /* if(!C){          +1/+2 */ \
    "out %[port], %[low]"  "\n\t" /*     port = low   +1    */ \
    "__L" X ":"            "\n\t" /* }                +0    */

#define PUF_WS2812_WRITE_BIT                                   \
    PUF_WS2812__WRITE_BIT(PUF_WS2812_STRINGIZE(__COUNTER__))

#define PUF_WS2812_WRITE_BIT_NOP                               \
    PUF_WS2812_WRITE_BIT   "\n\t" /*                  +4    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "out %[port], %[low]"  "\n\t" /* port = low       +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */

#define PUF_WS2812_WRITE_BIT_LAST                              \
    PUF_WS2812_WRITE_BIT   "\n\t" /*                  +4    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "out %[port], %[low]"  "\n\t" /* port = low       +1    */

#define PUF_WS2812_WRITE_BIT_JMP0                              \
    PUF_WS2812_WRITE_BIT   "\n\t" /*                  +4    */ \
    "jmp __Lhead"          "\n\t" /* goto head        +3    */

#define PUF_WS2812_WRITE_BIT_JMP1                              \
    "__Lhead:"             "\n\t" /* head:            +0    */ \
    "out %[port], %[low]"  "\n\t" /* port = low       +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */

#define PUF_WS2812_WRITE_BIT_BRE0                              \
    PUF_WS2812_WRITE_BIT   "\n\t" /*                  +4    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "nop"                  "\n\t" /*                  +1    */ \
    "cp %[ptr], %[end]"    "\n\t" /* ptr == end --> Z +1    */ \
    "out %[port], %[low]"  "\n\t" /* port = low       +1    */ \
    "breq __Lbreak"        "\n\t" /* if(Z) goto break +1/+2 */ \
    "nop"                  "\n\t" /*                  +1    */

#define PUF_WS2812_WRITE_BIT_BRE1                              \
    "__Lbreak:"            "\n\t" /* break:           +0    */

#define PUF_WS2812(NAME, PORT)                                 \
    void NAME(uint8_t* data, int len, uint8_t mask){           \
        volatile uint8_t  tmp  = 0;                            \
        volatile uint8_t* ptr  = data;                         \
        const    uint8_t* end  = data + len * 3;               \
        const    uint8_t  high = PORT |  mask;                 \
        const    uint8_t  low  = PORT & ~mask;                 \
                                                               \
        if(ptr == end){                                        \
            return;                                            \
        }                                                      \
                                                               \
        asm volatile(                                          \
            "jmp __Lbegin"            "\n\t" /*      [+3]   */ \
            PUF_WS2812_WRITE_BIT_JMP1 "\n\t" /*       +3    */ \
            PUF_WS2812_WRITE_BIT_NOP  "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_NOP  "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_BRE0 "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_LAST "\n\t" /*       +8    */ \
            "__Lbegin:"               "\n\t" /*      [+0]   */ \
            PUF_WS2812_LOAD_BYTE      "\n\t" /*       +2    */ \
            PUF_WS2812_WRITE_BIT_NOP  "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_NOP  "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_NOP  "\n\t" /*       +10   */ \
            PUF_WS2812_WRITE_BIT_JMP0 "\n\t" /*       +7    */ \
            PUF_WS2812_WRITE_BIT_BRE1 "\n\t" /*       +0    */ \
            PUF_WS2812_WRITE_BIT_LAST "\n\t" /*      [+8]   */ \
                                                               \
            :                                                  \
            [tmp]  "+r" (tmp),                                 \
            [ptr]  "+e" (ptr)                                  \
                                                               \
            :                                                  \
            [port] "I"  (_SFR_IO_ADDR(PORT)),                  \
            [high] "r"  (high),                                \
            [low]  "r"  (low),                                 \
            [end]  "e"  (end)                                  \
        );                                                     \
    }
