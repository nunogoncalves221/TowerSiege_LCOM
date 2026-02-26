#include "bitwise.h"
#include <stdarg.h> //para a func mask
#define TODO return 255
#define BIT(n) (1 << (n))

//desativar um bit - colocar o bit a 0
uint8_t clear(uint8_t msk, int pos) { 
    return msk & ~BIT(pos);
 }

 //ativar um bit - colocar o bit a 1
uint8_t set(uint8_t msk, int pos) { 
    return msk | BIT(pos);
}

//verificar se o bit está a 1
bool is_set(uint8_t msk, int pos) { 
    return (msk & BIT(pos)) != 0;
}

//least significant byte (o byte da direita)
uint8_t lsb(uint16_t wide_msk) {
    return (uint8_t)(wide_msk & 0xFF);
}

//most significant byte (o byte da esquerda)
uint8_t msb(uint16_t wide_msk) {
    return (uint8_t)(wide_msk >> 8);
 }

//criar uma máscara com vários bits
uint8_t mask(int pos, ...) {
    va_list args;
    va_start(args, pos);
    
    uint8_t final_mask = 0;
    int current_pos = pos;

    while (current_pos != -1) { 
        final_mask |= BIT(current_pos);
        current_pos = va_arg(args, int);
    }
    
    va_end(args);
    return final_mask;
}
