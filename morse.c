/*
 * Copyright (C) 2015. Joo, Young Jin <neoelec@gmail.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>

#include "morse.h"

#define ARRAY_SIZE(__arr)		(sizeof(__arr) / sizeof(__arr[0]))

/* [15:11] : morse code, 0 -> DIT, 1 -> DAH
 * [10:3]  : unused
 * [2:0]   : number of DIT/DAH
 *
 *         1         0
 *    5432109876543210
 *   +----------------
 * A |01---XXXXXXXX010
 * B |1000-XXXXXXXX100
 *          ...
 * 0 |01111XXXXXXXX101
 */

static const uint16_t morse_code_char[] PROGMEM = {
	0x4002,				/* A : .-    */
	0x8004,				/* B : -...  */
	0xA004,				/* C : -.-.  */
	0x8003,				/* D : -..   */
	0x0001,				/* E : .     */
	0x2004,				/* F : ..-.  */
	0xC003,				/* G : --.   */
	0x0004,				/* H : ....  */
	0x0002,				/* I : ..    */
	0x7004,				/* J : .---  */
	0xA003,				/* K : -.-   */
	0x4004,				/* L : .-..  */
	0xC002,				/* M : --    */
	0x8002,				/* N : -.    */
	0xE003,				/* O : ---   */
	0x6004,				/* P : .--.  */
	0xD004,				/* Q : --.-  */
	0x4003,				/* R : .-.   */
	0x0003,				/* S : ...   */
	0x8001,				/* T : -     */
	0x2003,				/* U : ..-   */
	0x1004,				/* V : ...-  */
	0x6003,				/* W : .--   */
	0x9004,				/* X : -..-  */
	0xB004,				/* Y : -.--  */
	0xC004,				/* Z : --..  */
};

static const uint16_t morse_code_num[] PROGMEM = {
	0xF805,				/* 0 : ----- */
	0x7805,				/* 1 : .---- */
	0x3805,				/* 2 : ..--- */
	0x1805,				/* 3 : ...-- */
	0x0805,				/* 4 : ....- */
	0x0005,				/* 5 : ..... */
	0x8005,				/* 6 : -.... */
	0xC005,				/* 7 : --... */
	0xE005,				/* 8 : ---.. */
	0xF005,				/* 9 : ----. */
};

struct char_code {
	char ch;
	uint16_t code;
};

/* Punctuation & Prosigns */
static const struct char_code morse_code_other[] PROGMEM = {
	/* Punctuation */
	{ '.', 0x5406, },		/* . : .-.-.- */
	{ ',', 0xCC06, },		/* , : --..-- */
	{ '?', 0x3006, },		/* ? : ..--.. */
	{ '\'', 0x7806, },		/* ' : .----. */
	{ '!', 0xAC06, },		/* ! : -.-.-- */
	{ '/', 0x9005, },		/* / : -..-. */
	{ '(', 0xB005, },		/* ( : -.--. */
	{ ')', 0xB406, },		/* ) : -.--.- */
	{ '&', 0x4005, },		/* & : .-... */
	{ ':', 0xE006, },		/* : : ---... */
	{ ';', 0xA806, },		/* ; : -.-.-. */
	{ '=', 0x8805, },		/* = : -...- */
	{ '+', 0x5005, },		/* + : .-.-. */
	{ '-', 0x8406, },		/* - : -....- */
	{ '_', 0x3406, },		/* _ : ..--.- */
	{ '"', 0x4806, },		/* " : .-..-. */
	{ '$', 0x1207, },		/* $ : ...-..- */
	{ '@', 0x6806, },		/* @ : .--.-. */
	/* Prosigns */
	{ ']', 0x1406, },		/* ] : ...-.- */
	{ 'e', 0x0007, },		/* e : ....... */
	{ 'i', 0xA003, },		/* i : -.- */
	{ '[', 0xA805, },		/* [ : -.-.- */
	{ 'u', 0x1005, },		/* u : ...-. */
	{ 'w', 0x4005, },		/* w : .-... */
};

uint16_t morse_char2code(const char ch)
{
	size_t i;

	switch (ch) {
	case 'A' ... 'Z':
		return pgm_read_word(&morse_code_char[ch - 'A']);
	case '0' ... '9':
		return pgm_read_word(&morse_code_num[ch - '0']);
	default:
		for (i = 0; ARRAY_SIZE(morse_code_other); i++)
			if (pgm_read_byte(&(morse_code_other[i].ch)) == ch)
				return pgm_read_word(&(morse_code_other[i].code));
	}

	return 'e';			/* e == ERROR */
}

void morse_encode(const char ch, void (*encode) (const char, const uint16_t))
{
	encode(ch, morse_char2code(ch));
}

/* based on magic morse algorithm
 * http://www.picaxeforum.co.uk/entry.php?30-Notes-behind-Magic-Morse */
static const char magic_morse[] PROGMEM =
	"_EISH5ee0TNDB6-0"		//   0 - 15	e == ERROR
	"00ARLw0000MGZ700"		//  16 - 31	w == WAIT
	"000UF0000i0KC000"		//  32 - 47	i == INVITE
	"000WP000000O0800"		//  48 - 63
	"0000Vu]00000X/00"		//  64 - 79	u == UNDERSTOOD  ] == End Of Work
	"00000+.00000Q000"		//  80 - 95
	"000000?00000Y()0"		//  96 - 111	() == Left/Right hand bracket
	"0000J0000000e900"		// 112 - 127
	"000004(c) M.R=BU"		// 128 - 143
	"RNETTE'0000000,0"		// 144 - 159	' @ [150] should be "
	"00>0000000000[00"		// 160 - 175	[ == Starting Signal
	"000000@000000000"		// 176 - 191
	"0000030000000000"		// 192 - 207
	"0000000000000000"		// 208 - 223
	"0000020000000000"		// 224 - 239
	"000001'000000000";		// 240 - 255

char morse_code2char(const uint16_t code)
{
	uint16_t mask = 1U << 15;
	size_t idx = 0U;
	size_t i;

	for (i = 0U; i < 3U; i++)
		idx += (code & (1U << i)) ? (1U << i) : 0U;

	for (i = 0U; i < (0x00FF & code); i++, mask >>= 1U)
		idx += (code & mask) ? 8U << i : 0U;

	return pgm_read_byte(&magic_morse[idx]);
}

void morse_decode(const uint16_t code,
		  void (*decode) (const uint16_t, const char))
{
	decode(code, morse_code2char(code));
}

#if 0	/* sample codes */
static void __morse_encode_sample(const char ch, const uint16_t code)
{
	uint16_t mask = 1U << 15;
	size_t i;
	char morse;

	printf("%c : ", ch);
	for (i = 0; i < (0x00FF & code); i++, mask >>= 1) {
		morse = (code & mask) ? '-' : '.';
		putchar(morse);
	}
	putchar('\n');
}

static void __morse_decode_sample(const uint16_t code, const char ch)
{
	printf("0x%04X : %c\n", code, ch);
}
#endif /* sample codes */
