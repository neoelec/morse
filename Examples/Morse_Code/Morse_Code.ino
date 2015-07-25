/*
 * Copyright (C) 2015. Joo, Young Jin <neoelec@gmail.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <LiquidCrystal.h>

#include "morse.h"

#define PIN_BUZZER			10
#define PIN_DIT_LED				13
#define PIN_DAH_LED				12

#define UNIT_TIME			75
#define TONE_DIT_LEN			(UNIT_TIME)
#define TONE_DAH_LEN			(UNIT_TIME * 3)
#define TONE_INTERVAL			(UNIT_TIME)
#define TONE_FREQ			750

#define CHAR_PAUSE			1000

static LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup(void)
{
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_DIT_LED, OUTPUT);
	pinMode(PIN_DAH_LED, OUTPUT);

	lcd.begin(16, 2);
	lcd.clear();
}

static void __morse_test_encode(const char ch, const uint16_t code)
{
	uint16_t mask = 1U << 15;
	size_t i;

	lcd.print("CHAR : ");
	lcd.print(ch);

	lcd.setCursor(0, 1);
	lcd.print("CODE : ");
	for (i = 0; i < (0x00FF & code); i++, mask >>= 1) {
		if (code & mask) {
			lcd.print('_');
			digitalWrite(PIN_DAH_LED, HIGH);
			tone(PIN_BUZZER, TONE_FREQ);
			delay(TONE_DAH_LEN);
			noTone(PIN_BUZZER);
			delay(TONE_INTERVAL);
		} else {
			lcd.print('.');
			digitalWrite(PIN_DIT_LED, HIGH);
			tone(PIN_BUZZER, TONE_FREQ);
			delay(TONE_DIT_LEN);
			noTone(PIN_BUZZER);
			delay(TONE_INTERVAL);
		}

	}
	digitalWrite(PIN_DIT_LED, LOW);
	digitalWrite(PIN_DAH_LED, LOW);

	delay(CHAR_PAUSE);
	lcd.clear();
}

void loop(void)
{
	char ch;

	for (ch = 'A'; ch <= 'Z'; ch++)
		morse_encode(ch, __morse_test_encode);

	for (ch = '0'; ch <= '9'; ch++)
		morse_encode(ch, __morse_test_encode);
}
