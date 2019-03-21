#pragma once

#include "utils.h"
#include <math.h>
#include <unistd.h> 
/**
* The representation of a piece is essentially
* a piano roll.
* A piece is just an array of units
*/

#define OCTAVES 4
#define TOTAL_TONES (OCTAVES * 12)
#define START_OCTAVE 3

#define TONE(note) ((note) & 63)
#define DURATION(note) ((note) & 3<<6)
#define NAME(note) "CcDdEFfGgAaB"[TONE(note)%12]
#define NOTE_OCTAVE(note) (TONE(note)/12+START_OCTAVE)

#define WHOLE_NOTE 3 << 6 
#define HALF_NOTE 2 << 6
#define QUARTER_NOTE 1 << 6
#define EIGHT_NOTE 0

#define SAME_DURATION(note1, note2) (TONE(note1) == TONE(note2))

/**
* A note uses the first 2 bits to store note duration
* and the other 6 bits to store the tone.
* 64 tones can be rapresented, good enough for 4 octaves 
*/
typedef unsigned char note;

/*
* A time unit of the piece.
* Represents an instant of time
*/
typedef struct
{
	note * accompaniement;
	unsigned int accompaniementCount;
	note * melody;
	unsigned int melodyCount;
} unit;

/*
* A time unit of the piece.
* Represents an instant of time
*/
typedef struct musical_piece
{
	unit * units;
	unsigned int unitsCount;
	unsigned int unitsPerSecond;
} piece;

note get_random_note();
float get_note_frequency(note note);
void make_random_piece(piece * p, unsigned int duration, unsigned int unitsPerSecond, unsigned int minNotesPerUnit, unsigned int maxNotesPerUnit, float melodyToAccNotesRatio01);
piece * get_musical_scale(unsigned int unitsPerSecond);
void print_piano_roll(piece * p);
void piece_free(piece * p);
void unit_free(unit * u);