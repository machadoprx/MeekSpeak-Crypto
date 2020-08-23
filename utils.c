#include "utils.h"

void
u32_to_u8(uint32_t *state, uint8_t *stream, unsigned len)
{
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 4; j++) {
            int stream_index = i * 4 + (3 - j);
            stream[stream_index] = (uint8_t)((state[i] >> (j * 8)) & 0xFF);
        }
    }
}

void
u8_to_u32(uint8_t *stream, uint32_t *arr, unsigned len)
{
    for (int i = 0; i < len; i++) {
        uint32_t acc = 0;
        uint32_t shift = 0;
        for (int j = 3; j >= 0; j--) {
            acc += (uint32_t)(stream[i * 4 + j] << shift);
            shift += 8;
        }
        arr[i] = acc;
    }
}