#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

void serialize(uint32_t state[], uint8_t stream[], unsigned len);
void byte_to_array(uint8_t *stream, uint32_t *arr);

#endif