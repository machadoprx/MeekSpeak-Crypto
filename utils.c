#include "utils.h"

void
serialize(uint32_t state[], uint8_t stream[], unsigned len)
{
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 4; j++) {
            int stream_index = (len - 1 - i) * 4 + (3 - j);
            stream[stream_index] = (uint8_t)((state[i] >> (j * 8)) & 0xFF);
        }
    }
}

void
byte_to_array(uint8_t *stream, uint32_t *arr)
{
    for (int i = 0; i < 4; i++) {
        uint32_t acc = 0;
        uint32_t shift = 0;
        for (int j = 3; j >= 0; j--) {
            acc += (stream[i * 4 + j] << shift);
            shift += 8;
        }
        arr[i] = acc;
    }
}