#include "utils.h"

void
u32_to_u8(uint32_t *state, uint8_t *stream, unsigned len)
{
    for (int i = len - 1; i >= 0; i--) {
        uint32_t curr = state[i];
        for (int j = 3; j >= 0; j--) {
            stream[(len - 1 - i) * 4 + (3 - j)] = (uint8_t)(curr >> (8 * j));
        }
    }
}

void
u8_to_u32(uint8_t *stream, uint32_t *arr, unsigned len)
{
    for (int i = 0; i < len; i++) {
        uint32_t sum = 0;
        for (int j = 0; j < 4; j++) {
            sum += (uint32_t)stream[i * 4 + j] << (8 * j);
        }
        arr[i] = sum;
    }
}