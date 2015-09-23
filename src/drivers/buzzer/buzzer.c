/*
 * drivers/buzzer/buzzer.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <buzzer/buzzer.h>

static void buzzer_suspend(struct device_t * dev)
{
	struct buzzer_t * buzzer;

	if(!dev || dev->type != DEVICE_TYPE_BUZZER)
		return;

	buzzer = (struct buzzer_t *)(dev->driver);
	if(!buzzer)
		return;

	if(buzzer->suspend)
		buzzer->suspend(buzzer);
}

static void buzzer_resume(struct device_t * dev)
{
	struct buzzer_t * buzzer;

	if(!dev || dev->type != DEVICE_TYPE_BUZZER)
		return;

	buzzer = (struct buzzer_t *)(dev->driver);
	if(!buzzer)
		return;

	if(buzzer->resume)
		buzzer->resume(buzzer);
}

static ssize_t buzzer_read_frequency(struct kobj_t * kobj, void * buf, size_t size)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)kobj->priv;
	int frequency;

	frequency = buzzer_get_frequency(buzzer);
	return sprintf(buf, "%d", frequency);
}

static ssize_t buzzer_write_frequency(struct kobj_t * kobj, void * buf, size_t size)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)kobj->priv;
	int frequency = strtol(buf, NULL, 0);

	buzzer_set_frequency(buzzer, frequency);
	return size;
}

static ssize_t buzzer_write_play(struct kobj_t * kobj, void * buf, size_t size)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)kobj->priv;

	buzzer_play(buzzer, buf);
	return size;
}

struct buzzer_t * search_buzzer(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_BUZZER);
	if(!dev)
		return NULL;

	return (struct buzzer_t *)dev->driver;
}

struct buzzer_t * search_first_buzzer(void)
{
	struct device_t * dev;

	dev = search_first_device_with_type(DEVICE_TYPE_BUZZER);
	if(!dev)
		return NULL;

	return (struct buzzer_t *)dev->driver;
}

bool_t register_buzzer(struct buzzer_t * buzzer)
{
	struct device_t * dev;

	if(!buzzer || !buzzer->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(buzzer->name);
	dev->type = DEVICE_TYPE_BUZZER;
	dev->suspend = buzzer_suspend;
	dev->resume = buzzer_resume;
	dev->driver = buzzer;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "frequency", buzzer_read_frequency, buzzer_write_frequency, buzzer);
	kobj_add_regular(dev->kobj, "play", NULL, buzzer_write_play, buzzer);

	if(buzzer->init)
		(buzzer->init)(buzzer);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_buzzer(struct buzzer_t * buzzer)
{
	struct device_t * dev;
	struct buzzer_t * driver;

	if(!buzzer || !buzzer->name)
		return FALSE;

	dev = search_device_with_type(buzzer->name, DEVICE_TYPE_BUZZER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct buzzer_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(buzzer);

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void buzzer_set_frequency(struct buzzer_t * buzzer, int frequency)
{
	if(buzzer && buzzer->set)
	{
		if(frequency < 0)
			frequency = 0;
		buzzer->set(buzzer, frequency);
	}
}

int buzzer_get_frequency(struct buzzer_t * buzzer)
{
	if(buzzer && buzzer->get)
		return buzzer->get(buzzer);
	return 0;
}

void buzzer_beep(struct buzzer_t * buzzer, int frequency, int millisecond)
{
	if(buzzer && buzzer->beep)
	{
		if(frequency < 0)
			frequency = 0;
		if(millisecond < 0)
			millisecond = 0;
		buzzer->beep(buzzer, frequency, millisecond);
	}
}

enum {
	NOTE_C0		= 16,
	NOTE_CS0	= 17,
	NOTE_D0		= 18,
	NOTE_DS0	= 20,
	NOTE_E0		= 21,
	NOTE_F0		= 22,
	NOTE_FS0	= 23,
	NOTE_G0		= 25,
	NOTE_GS0	= 26,
	NOTE_A0		= 28,
	NOTE_AS0	= 29,
	NOTE_B0		= 31,

	NOTE_C1		= 33,
	NOTE_CS1	= 35,
	NOTE_D1		= 37,
	NOTE_DS1	= 39,
	NOTE_E1		= 41,
	NOTE_F1		= 44,
	NOTE_FS1	= 46,
	NOTE_G1		= 49,
	NOTE_GS1	= 52,
	NOTE_A1		= 55,
	NOTE_AS1	= 58,
	NOTE_B1		= 62,

	NOTE_C2		= 65,
	NOTE_CS2	= 69,
	NOTE_D2		= 73,
	NOTE_DS2	= 78,
	NOTE_E2		= 82,
	NOTE_F2		= 87,
	NOTE_FS2	= 93,
	NOTE_G2		= 98,
	NOTE_GS2	= 104,
	NOTE_A2		= 110,
	NOTE_AS2	= 117,
	NOTE_B2		= 123,

	NOTE_C3		= 131,
	NOTE_CS3	= 139,
	NOTE_D3		= 147,
	NOTE_DS3	= 156,
	NOTE_E3		= 165,
	NOTE_F3		= 175,
	NOTE_FS3	= 185,
	NOTE_G3		= 196,
	NOTE_GS3	= 208,
	NOTE_A3		= 220,
	NOTE_AS3	= 233,
	NOTE_B3		= 247,

	NOTE_C4		= 262,
	NOTE_CS4	= 277,
	NOTE_D4		= 294,
	NOTE_DS4	= 311,
	NOTE_E4		= 330,
	NOTE_F4		= 349,
	NOTE_FS4	= 370,
	NOTE_G4		= 392,
	NOTE_GS4	= 415,
	NOTE_A4		= 440,
	NOTE_AS4	= 466,
	NOTE_B4		= 494,

	NOTE_C5		= 523,
	NOTE_CS5	= 554,
	NOTE_D5		= 587,
	NOTE_DS5	= 622,
	NOTE_E5		= 659,
	NOTE_F5		= 698,
	NOTE_FS5	= 740,
	NOTE_G5		= 784,
	NOTE_GS5	= 831,
	NOTE_A5		= 880,
	NOTE_AS5	= 932,
	NOTE_B5		= 988,

	NOTE_C6		= 1047,
	NOTE_CS6	= 1109,
	NOTE_D6		= 1175,
	NOTE_DS6	= 1245,
	NOTE_E6		= 1319,
	NOTE_F6		= 1397,
	NOTE_FS6	= 1480,
	NOTE_G6		= 1568,
	NOTE_GS6	= 1661,
	NOTE_A6		= 1760,
	NOTE_AS6	= 1865,
	NOTE_B6		= 1976,

	NOTE_C7		= 2093,
	NOTE_CS7	= 2217,
	NOTE_D7		= 2349,
	NOTE_DS7	= 2489,
	NOTE_E7		= 2637,
	NOTE_F7		= 2794,
	NOTE_FS7	= 2960,
	NOTE_G7		= 3136,
	NOTE_GS7	= 3322,
	NOTE_A7		= 3520,
	NOTE_AS7	= 3729,
	NOTE_B7		= 3951,

	NOTE_C8		= 4186,
	NOTE_CS8	= 4435,
	NOTE_D8		= 4699,
	NOTE_DS8	= 4978,
};

/*
 * RTTTL (RingTone Text Transfer Language) is the primary format used to distribute
 * ringtones for Nokia phones. An RTTTL file is a text file, containing the
 * ringtone name, a control section and a section containing a comma separated
 * sequence of ring tone commands. White space must be ignored by any reader
 * application.
 *
 * Example:
 * Simpsons:d=4,o=5,b=160:32p,c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g
 *
 * This file describes a ringtone whose name is 'Simpsons'. The control section
 * sets the beats per minute at 160, the default note length as 4, and the default
 * scale as Octave 5.
 * <RTX file> := <name> ":" [<control section>] ":" <tone-commands>
 *
 * 	<name> := <char> ; maximum name length 10 characters
 *
 * 	<control-section> := <control-pair> ["," <control-section>]
 *
 * 		<control-pair> := <control-name> ["="] <control-value>
 *
 * 		<control-name> := "d" | "o" | "b"
 * 		; Valid in control section: d=default duration, o=default scale, b=default beats per minute.
 * 		; if not specified, defaults are 4=duration, 6=scale, 63=beats-per-minute
 * 		; any unknown control-names must be ignored
 *
 * 		<tone-commands> := <tone-command> ["," <tone-commands>]
 *
 * 		<tone-command> :=<note> | <control-pair>
 *
 * 		<note> := [<duration>] <note> [<scale>] [<special-duration>] <delimiter>
 *
 * 			<duration> := "1" | "2" | "4" | "8" | "16" | "32"
 * 			; duration is divider of full note duration, eg. 4 represents a quarter note
 *
 * 			<note> := "P" | "C" | "C#" | "D" | "D#" | "E" | "F" | "F#" | "G" | "G#" | "A" | "A#" | "B"
 *
 * 			<scale> :="4" | "5" | "6" | "7"
 * 			; Note that octave 4: A=440Hz, 5: A=880Hz, 6: A=1.76 kHz, 7: A=3.52 kHz
 * 			; The lowest note on the Nokia 61xx is A4, the highest is B7
 *
 * 			<special-duration> := "." ; Dotted note
 */
void buzzer_play(struct buzzer_t * buzzer, const char * rtttl)
{
	const int notes[] = { 0,
		NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
		NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
		NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
		NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
	};
	char * p = (char *)rtttl;
	int d = 4, o = 6, b = 63;
	int index, scale, duration;
	int n, t;

	if(!buzzer || !rtttl)
		return;
	buzzer_beep(buzzer, 0, 0);

	while(*p != ':')
		p++;
	p++;

	if(*p == 'd')
	{
		p++;
		p++;
		n = 0;
		while(isdigit(*p))
		{
			n = (n * 10) + (*p++ - '0');
		}
		if(n > 0)
			d = n;
		p++;
	}

	if(*p == 'o')
	{
		p++;
		p++;
		n = *p++ - '0';
		if(n >= 4 && n <= 7)
			o = n;
		p++;
	}

	if(*p == 'b')
	{
		p++;
		p++;
		n = 0;
		while(isdigit(*p))
		{
			n = (n * 10) + (*p++ - '0');
		}
		b = n;
		p++;
	}

	t = (4 * 60 * 1000) / b;

	while(*p)
	{
		n = 0;
		while(isdigit(*p))
		{
			n = (n * 10) + (*p++ - '0');
		}

		if(n)
			duration = t / n;
		else
			duration = t / d;

		index = 0;
		switch (*p)
		{
		case 'c':
			index = 1;
			break;
		case 'd':
			index = 3;
			break;
		case 'e':
			index = 5;
			break;
		case 'f':
			index = 6;
			break;
		case 'g':
			index = 8;
			break;
		case 'a':
			index = 10;
			break;
		case 'b':
			index = 12;
			break;
		case 'p':
		default:
			index = 0;
		}
		p++;

		if(*p == '#')
		{
			index++;
			p++;
		}

		if(*p == '.')
		{
			duration += duration / 2;
			p++;
		}

		if(isdigit(*p))
		{
			scale = *p - '0';
			p++;
		}
		else
		{
			scale = o;
		}

		if(*p == ',')
			p++;

		if(index != 0)
			buzzer_beep(buzzer, notes[(scale - 4) * 12 + index], duration);
		else
			buzzer_beep(buzzer, 0, duration);
		buzzer_beep(buzzer, 0, 10);
	}
}
