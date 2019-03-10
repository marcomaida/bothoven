#include "utils.h"

/**
* The representation of a piece is essentially
* a piano roll.
* A piece is just an array of units
*/

#define OCTAVES 4
#define TOTAL_TONES (OCTAVES * 12)

/**
* A note uses the first 2 bits to store type (#TODO)
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
typedef struct
{
	unit * units;
	unsigned int unitsCount;
} piece;

note get_random_note()
{
	return rand() % TOTAL_TONES;
}

piece * get_random_piece(unsigned int duration, unsigned int unitsPerSecond, unsigned int minNotesPerUnit, unsigned int maxNotesPerUnit, float melodyToAccNotesRatio01)
{
	piece * p = NEW(piece);
	p->unitsCount = duration * unitsPerSecond;
	p->units = NEW_ARRAY(unit,p->unitsCount);

	for(unsigned int i = 0; i < p->unitsCount; i++)
	{//building every unit
		unit * u = &(p->units[i]);
		unsigned int notesCount = rand() % (maxNotesPerUnit - minNotesPerUnit) + minNotesPerUnit;
		
		u->melodyCount = notesCount * melodyToAccNotesRatio01;
		u->melody = NEW_ARRAY(note, u->melodyCount);
		for(unsigned int ii = 0; ii < u->melodyCount; ii++)
		{//building every melody note of a unit
			u->melody[ii] = get_random_note();
		}

		u->accompaniementCount = notesCount - u->melodyCount;
		u->accompaniement = NEW_ARRAY(note, u->accompaniementCount);
		for(unsigned int ii = 0; ii < u->accompaniementCount; ii++)
		{//building every accompaniement note of a unit
			u->accompaniement[ii] = get_random_note();
		}
	}

	return p;
}

void print_piano_roll(piece * p)
{
	char notes [TOTAL_TONES +1];
	notes[TOTAL_TONES] = '\0';

	for(unsigned int i = 0; i < p->unitsCount; i++)
	{
		memset(notes, ' ', (TOTAL_TONES));

		unit * u = &(p->units[i]);
		for(unsigned int ii = 0; ii < u->melodyCount; ii++)
		{
			notes[u->melody[ii]] = 'O';
		}

		for(unsigned int ii = 0; ii < u->accompaniementCount; ii++)
		{//building every accompaniement note of a unit
			notes[u->accompaniement[ii]] = 'O';
		}

		printf("|%s|\n", notes);
	}
}