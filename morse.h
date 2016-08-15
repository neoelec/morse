/*
 * Copyright (C) 2015. Joo, Young Jin <neoelec@gmail.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MORSE_H__
#define __MORSE_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

uint16_t morse_char2code(const char ch);

bool morse_encode(const char ch,
		void (*encode) (const char, const uint16_t));

char morse_code2char(const uint16_t code);

bool morse_decode(const uint16_t code,
		void (*decode) (const uint16_t, const char));

#ifdef __cplusplus
}
#endif

#endif /* __MORSE_H__ */
