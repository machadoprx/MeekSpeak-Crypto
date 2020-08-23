#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void u32_to_u8(uint32_t *state, uint8_t *stream, unsigned len);
void u8_to_u32(uint8_t *stream, uint32_t *arr, unsigned len);

#endif