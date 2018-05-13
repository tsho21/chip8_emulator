#pragma once
#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>

#define NTH_BIT_OF_BYTE(b, bit) (((b) >> (bit)) & 1)

typedef uint8_t uint8;
typedef uint16_t uint16;

#endif
